#include "environment.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <thread>


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
  for (Body* body: mo_bodies)
  {
    body->oneStep();
  }
  ++m_currentTime;
}


std::array<double, 4>
Environment::getStateDerivative(const std::array<double, 4>& x, Body* ignoreBody, unsigned int time)
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
    if (r > 0.1/std::sqrt(gravitationalConstant))
    {
      double mu = mass * gravitationalConstant;
      double r3 = r*r*r;
      stateDerivative[2] -= mu / r3 * x12;
      stateDerivative[3] -= mu / r3 * y12;
    }
    else
    {
      // TODO: make a better rule
      std::cout << "WARNING- Environment::getStateDerivative() : close approach, skip force calculation" << std::endl;
      std::cout << "limit: " << 1.0/std::sqrt(gravitationalConstant) << std::endl;
      std::cout << "distance: " << r << std::endl;
    }
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
