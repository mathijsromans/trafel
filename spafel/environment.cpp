#include "environment.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <thread>


Environment::Environment(double gravitationalConstant)
: m_gravitationalConstant(gravitationalConstant),
  mo_bodies(),
  mo_masslessBodies()
{
}


Environment::~Environment()
{
  for (Body* body : mo_bodies)
  {
    delete body;
  }
  mo_bodies.clear();

  for (Body* body : mo_masslessBodies)
  {
    delete body;
  }
  mo_masslessBodies.clear();
}


void
Environment::addBody(Body* body)
{
  mo_bodies.push_back(body);
}


void
Environment::removeBody(Body* body)
{
  mo_bodies.erase( std::remove(mo_bodies.begin(), mo_bodies.end(), body), mo_bodies.end() );
}


void
Environment::addMasslessBody(Body* body)
{
  mo_masslessBodies.push_back(body);
}


void
Environment::clearAllBodies()
{
  mo_bodies.clear();
  mo_masslessBodies.clear();
}

const std::vector<Body*>&
Environment::getBodies() const
{
  return mo_bodies;
}


void
Environment::oneStep(double tEnd, double stepsize)
{
  double stepsizeAbsolute = std::fabs(stepsize);
  if (stepsizeAbsolute < 1.0e-6)
  {
    std::cerr << "Environment::oneStepImpl() - ERROR: stepsize too small: " << stepsize << std::endl;
    return;
  }

  int steps = tEnd/stepsizeAbsolute;
  assert(steps >= 0);

  for (int i = 0; i < steps; ++i)
  {
    for (Body* body: mo_bodies)
    {
      body->oneStep(stepsize);
    }
    for (Body* body: mo_masslessBodies)
    {
      body->oneStep(stepsize);
    }
  }

  for (Body* body: mo_bodies)
  {
    body->updateTrack();
  }
}


std::array<double, 4>
Environment::getStateDerivative(const std::array<double, 4>& x, Body* ignoreBody)
{
  std::array<double, 4> stateDerivative;
  stateDerivative.fill(0.0);

  for (auto iter = mo_bodies.begin(); iter != mo_bodies.end(); iter++)
  {
    if (ignoreBody == *iter)
    {
      continue;
    }

    const std::array<double, 4>& x2 = (*iter)->getState();
    double x12 = x[0]-x2[0];
    double y12 = x[1]-x2[1];
    double r = std::sqrt(x12*x12+y12*y12);
    if (r > 0.1/std::sqrt(m_gravitationalConstant))
    {
      double mu = (*iter)->getMass() * m_gravitationalConstant;
      double r3 = r*r*r;
      stateDerivative[2] -= mu / r3 * x12;
      stateDerivative[3] -= mu / r3 * y12;
    }
    else
    {
      // TODO: make a better rule
      std::cout << "WARNING- Environment::getStateDerivative() : close approach, skip force calculation" << std::endl;
      std::cout << "limit: " << 1.0/std::sqrt(m_gravitationalConstant) << std::endl;
      std::cout << "distance: " << r << std::endl;
    }
  }

  stateDerivative[0] = x[2];
  stateDerivative[1] = x[3];
  return stateDerivative;
}


double
Environment::getFieldStrength(double x, double y)
{
  std::array<double, 4> x2;
  double x12[2];
  double gravityAcc = 0.0;
  double gravityAccX = 0.0;
  double gravityAccY = 0.0;

  for (auto iter = mo_bodies.begin(); iter != mo_bodies.end(); iter++)
  {
    x2 = (*iter)->getState();
    if (x2[0] != x && x2[1] != y)
    {
      double mass = (*iter)->getMass();
      x12[0] = x-x2[0];
      x12[1] = y-x2[1];
      double r = sqrt(x12[0]*x12[0]+x12[1]*x12[1]);
      if (r > 1)
      {
        gravityAccX += mass/(r*r*r)*x12[0];
        gravityAccY += mass/(r*r*r)*x12[1];
      }
    }
  }

  gravityAcc = sqrt( pow(gravityAccX, 2) + pow(gravityAccY, 2) );
  return gravityAcc;
}


double
Environment::getGravitationalConstant() const
{
  return m_gravitationalConstant;
}

