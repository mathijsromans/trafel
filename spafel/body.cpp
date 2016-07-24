#include "environment.h"

#include "body.h"
#include <cassert>

Body::Body(double x, double y, double vx, double vy, double mass, Environment* environment)
: m_x(),
  m_trackChanged(true),
  m_currentTime(0),
  m_mass(mass),
  m_environment(environment)
{
  m_x[0][0] = x;
  m_x[0][1] = y;
  m_x[0][2] = vx;
  m_x[0][3] = vy;
}


Body::~Body()
{
}


void
Body::oneStep()
{  
  ++m_currentTime;
  calculate(m_currentTime+m_x.size()-1);
}

void
Body::calculate(unsigned int time)
{
  assert(time > m_currentTime);
  assert(time < m_currentTime + m_x.size());
  double stepsize = m_environment->getStepsize();
  const std::array<double, 4> x = getState(time-1);
  std::array<double, 4> force = m_environment->getStateDerivative(x, this, time-1);

  std::array<double, 4> x0k1;
  std::array<double, 4> x0k2;
  std::array<double, 4> x0k3;
  std::array<double, 4> k1;
  std::array<double, 4> k2;
  std::array<double, 4> k3;
  std::array<double, 4> k4;

  for (std::size_t j = 0; j < 4; j++)
  {
    k1[j] = stepsize * force[j];
    x0k1[j] = x[j] + 1./2.* k1[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k1, this, time-1);
  for (std::size_t j = 0; j < 4; j++)
  {
    k2[j] = stepsize * force[j];
    x0k2[j] = x[j] + 1./2.* k2[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k2, this, time-1);
  for (std::size_t j = 0; j < 4; j++)
  {
    k3[j] = stepsize * force[j];
    x0k3[j] = x[j] + k3[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k3, this, time-1);

  for (std::size_t j = 0; j < 4; j++)
  {
    k4[j] = stepsize * force[j];
  }

  for (std::size_t i = 0; i < 4; i++)
  {
    m_x[time%m_x.size()][i] = x[i] + 1.0/6.0 * (k1[i]+2*k2[i]+2*k3[i]+k4[i]);
  }
}

double
Body::getMass() const
{
  return m_mass;
}


const std::array<double, 4>&
Body::getCurrentState() const
{
  return getState(m_currentTime);
}

const std::array<double, 4>&
Body::getState(unsigned int time) const
{
  assert(time >= m_currentTime);
  assert(time < m_currentTime+m_x.size());
  return m_x[time%m_x.size()];
}

void Body::boost(Body::Direction d)
{
  double& vx = m_x[m_currentTime%m_x.size()][2];
  double& vy = m_x[m_currentTime%m_x.size()][3];
  switch (d)
  {
    case Direction::up: vx *= 1.1; vy *= 1.1; break;
    case Direction::down:break;
    case Direction::left:break;
    case Direction::right:break;
  }

  for ( unsigned int t = 1; t != m_x.size(); ++t )
  {
    calculate(m_currentTime+t);
  }

  m_trackChanged = true;
}

bool Body::trackChanged()
{
  bool result = m_trackChanged;
  m_trackChanged = false;
  return result;
}
