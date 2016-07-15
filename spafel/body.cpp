#include "integrator.h"
#include "environment.h"

#include "body.h"

unsigned int Body::ms_nextUniqueId = 1;

Body::Body(Environment* environment)
: m_id(ms_nextUniqueId++),
  m_x(),
  m_para(),
  m_integrator(),
  m_environment(environment),
  m_nSteps(0)
{
  m_x[0] = 200.0;
  m_x[1] = 200.0;
  m_x[2] = 0;
  m_x[3] = 0;

  m_para[0] = 1.0; // mass [kg]
  m_para[1] = 15.0; // radius [m]

  m_integrator.reset(new Integrator(m_environment, this));
}


Body::~Body()
{
}


void
Body::oneStep(double stepsize)
{
  m_x = m_integrator->integrate(stepsize);
  m_nSteps++;
}


void
Body::setPosition(double xin, double yin)
{
  m_x[0] = xin;
  m_x[1] = yin;
  m_integrator.reset(new Integrator(m_environment, this));
}


void
Body::setVelocity(double vxin, double vyin)
{
  m_x[2] = vxin;
  m_x[3] = vyin;
  m_integrator.reset(new Integrator(m_environment, this));
}


void
Body::setMass(double mass)
{
  m_para[0] = mass;
}


double
Body::getMass() const
{
  return m_para[0];
}


const std::array<double, 4>&
Body::getState() const
{
  return m_x;
}


const std::array<double, 2>&
Body::getParameters() const
{
  return m_para;
}


void
Body::printPosition() const
{
  double distance = std::sqrt(m_x[0]*m_x[0]+m_x[1]*m_x[1]);
  std::cout << "pos: " << m_x[0] << ", " << m_x[1] << ", distance: " << distance << std::endl;
}

void
Body::printVelocity() const
{
  double speed = std::sqrt(m_x[2]*m_x[2]+m_x[3]*m_x[3]);
  std::cout << "vel: " << m_x[2] << ", " << m_x[3] << ", speed: " << speed << std::endl;
}

unsigned int
Body::getId() const
{
  return m_id;
}


double
Body::random(double start, double end)
{
  double randomDouble = start+(end-start)*rand()/(RAND_MAX + 1.0);
  return randomDouble;
}

