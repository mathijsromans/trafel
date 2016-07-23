#include "environment.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <thread>


Environment::Environment(double gravitationalConstant)
: m_gravitationalConstant(gravitationalConstant),
  m_lastTime(0),
  mo_bodies()
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

const std::vector<Body*>&
Environment::getBodies() const
{
  return mo_bodies;
}


void
Environment::oneStep(double stepsize, unsigned int time)
{
  while ( m_lastTime < time + timeAhead )
  {
    double stepsizeAbsolute = std::fabs(stepsize);
    if (stepsizeAbsolute < 1.0e-6)
    {
      std::cerr << "Environment::oneStepImpl() - ERROR: stepsize too small: " << stepsize << std::endl;
      return;
    }

    for (Body* body: mo_bodies)
    {
      body->oneStep(stepsize, m_lastTime);
    }
    ++m_lastTime;
  }

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
    if (r > 0.1/std::sqrt(m_gravitationalConstant))
    {
      double mu = mass * m_gravitationalConstant;
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
