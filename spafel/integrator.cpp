#include "integrator.h"


Integrator::Integrator(Environment* environment, Body* body)
: m_environment(environment),
  m_body(body),
  x(body->getState())
{
}


Integrator::~Integrator()
{
//  std::cout << "Integrator::~Integrator()" << std::endl;
}


const std::array<double, 4>&
Integrator::integrate(double stepsize)
{
  oneStep(stepsize);
  return x;
}


void
Integrator::oneStep(double stepsize)
{
  std::array<double, 4> force = m_environment->getStateDerivative(x, m_body);
  for (std::size_t j = 0; j < x.size(); j++)
  {
    k1[j] = stepsize * force[j];
    x0k1[j] = x[j] + 1./2.* k1[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k1, m_body);
  for (std::size_t j = 0; j < x.size(); j++)
  {
    k2[j] = stepsize * force[j];
    x0k2[j] = x[j] + 1./2.* k2[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k2, m_body);
  for (std::size_t j = 0; j < x.size(); j++)
  {
    k3[j] = stepsize * force[j];
    x0k3[j] = x[j] + k3[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k3, m_body);

  for (std::size_t j = 0; j < x.size(); j++)
  {
    k4[j] = stepsize * force[j];
  }

  for (std::size_t i = 0; i < x.size(); i++)
  {
    x[i] = x[i] + 1.0/6.0 * (k1[i]+2*k2[i]+2*k3[i]+k4[i]);
  }
}
