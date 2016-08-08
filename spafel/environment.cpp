#include "environment.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <thread>
#include <set>

namespace
{
  const double minDistance = 5.0e9;

  template <typename T> int sign(T val)
  {
    return (T(0) < val) - (val < T(0));
  }
}

Environment::Environment(unsigned int stepsize)
: m_stepsize(stepsize),
  m_currentTime(0),
  mo_bodies(),
  m_spaceships()
{
}

Environment::~Environment()
{
  for (Body* body : mo_bodies)
  {
    delete body;
  }
  mo_bodies.clear();
}


void
Environment::addBody(Body* body)
{
  mo_bodies.push_back(body);
}

void
Environment::addSpaceship(Body* body)
{
  addBody(body);
  m_spaceships.push_back(body);
}

void Environment::init()
{
  assert(m_currentTime == 0);
  for ( unsigned int time = 1; time != Body::timeAhead; ++time )
  {
    for (Body* body: mo_bodies)
    {
      body->calculate(time);
    }
  }
}

const std::vector<Body*>&
Environment::getBodies() const
{
  return mo_bodies;
}

void
Environment::oneStep()
{
  preventCollisions();

  for (Body* body: mo_bodies)
  {
    body->oneStep();
  }

  ++m_currentTime;
}


std::array<double, 4>
Environment::getStateDerivative(const std::array<double, 4>& x, const Body* ignoreBody, unsigned int time) const
{
  std::array<double, 4> stateDerivative;
  stateDerivative.fill(0.0);

  for (Body* body : mo_bodies )
  {
    double mass = body->getMass();
    if (ignoreBody == body || mass == 0)
    {
      continue;
    }

    const std::array<double, 4>& x2 = body->getState(time);
    double x12 = x[0]-x2[0];
    double y12 = x[1]-x2[1];

    double r = std::sqrt(x12*x12+y12*y12);
    if (r < minDistance)
    {
      continue;
    }
    double mu = mass * gravitationalConstant;
    double r3 = r*r*r;
    stateDerivative[2] -= mu / r3 * x12;
    stateDerivative[3] -= mu / r3 * y12;
  }

  stateDerivative[0] = x[2];
  stateDerivative[1] = x[3];
  return stateDerivative;
}

void Environment::boost(unsigned int spaceshipId, Body::Direction d)
{
  assert(spaceshipId < m_spaceships.size());
  m_spaceships[spaceshipId]->boost(d);
}

unsigned int Environment::getStepsize() const
{
  return m_stepsize;
}

unsigned int Environment::getCurrentTime() const
{
  return m_currentTime;
}

double Environment::calcOrbitalVelocity(double apogeeX, double eccentricity, double massCentralBody)
{
  double apogeeVy = std::sqrt(gravitationalConstant*massCentralBody/apogeeX*(1.0-eccentricity));
  return apogeeVy;
}

void Environment::preventCollisions() const
{
  std::set<Body*> bodiesMoved;
  for (Body* bodyA: mo_bodies)
  {
    unsigned int time = m_currentTime + Body::timeAhead - 1;
    for (Body* bodyB: mo_bodies)
    {
      if (bodyA == bodyB || bodiesMoved.find(bodyA) != bodiesMoved.end() || bodiesMoved.find(bodyB) != bodiesMoved.end())
      {
        continue;
      }

      Body* body1 = bodyA;
      Body* body2 = bodyB;
      if (bodyA->getMass() > bodyB->getMass())
      {
        body1 = bodyB;
        body2 = bodyA;
      }
      const std::array<double, 4>& x1 = body1->getState(time);
      const std::array<double, 4>& x2 = body2->getState(time);
      double dx = x2[0]-x1[0];
      double dy = x2[1]-x1[1];
      double dvx = (x2[2]-x1[2]) * -sign(dx);
      double dvy = (x2[3]-x1[3]) * -sign(dy);
      double dv = dvx + dvy;

      double r = std::sqrt(dx*dx+dy*dy);
      if (r < minDistance && dv > 0.0)
      {
        const std::array<double, 4>& x = body1->getState(time);
        std::array<double, 4> xNew;
        xNew[0] = x[0] + 2.0*dx;
        xNew[1] = x[1] + 2.0*dy;
        xNew[2] = x[2]*1.01;
        xNew[3] = x[3]*1.01;
        body1->setState(xNew, time);
        bodiesMoved.insert(body1);
      }
    }
  }
}
