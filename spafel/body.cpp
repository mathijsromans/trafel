#include "environment.h"

#include "body.h"
#include <cassert>

Body::Body(double x, double y, double vx, double vy, double mass, Environment* environment)
: m_x(),
  m_lastTime(0),
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
Body::oneStep(unsigned int time)
{
  double stepsize = m_environment->getStepsize();
  assert(time == m_lastTime);
  std::array<double, 4> x = getState(time);
  std::array<double, 4> force = m_environment->getStateDerivative(x, this, time);

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

  force = m_environment->getStateDerivative(x0k1, this, time);
  for (std::size_t j = 0; j < 4; j++)
  {
    k2[j] = stepsize * force[j];
    x0k2[j] = x[j] + 1./2.* k2[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k2, this, time);
  for (std::size_t j = 0; j < 4; j++)
  {
    k3[j] = stepsize * force[j];
    x0k3[j] = x[j] + k3[j]; // used in next step
  }

  force = m_environment->getStateDerivative(x0k3, this, time);

  for (std::size_t j = 0; j < 4; j++)
  {
    k4[j] = stepsize * force[j];
  }

  for (std::size_t i = 0; i < 4; i++)
  {
    m_x[(m_lastTime+1)%m_x.size()][i] = x[i] + 1.0/6.0 * (k1[i]+2*k2[i]+2*k3[i]+k4[i]);
  }
  m_lastTime++;
}

double
Body::getMass() const
{
  return m_mass;
}


const std::array<double, 4>&
Body::getState(unsigned int time) const
{
  assert(time<=m_lastTime);
  assert(time+m_x.size()>m_lastTime);
  return m_x[time%m_x.size()];
}

void Body::boost(Body::Direction d)
{
  assert( m_lastTime > m_x.size() );
  unsigned int currentTime = m_lastTime+1-m_x.size();
  double& vx = m_x[currentTime%m_x.size()][2];
  double& vy = m_x[currentTime%m_x.size()][3];
  switch (d)
  {
    case Direction::up: vx *= 1.1; vy *= 1.1; break;
    case Direction::down:break;
    case Direction::left:break;
    case Direction::right:break;
  }

  m_lastTime = currentTime;
  for ( unsigned int i = 0; i != m_x.size()-1; ++i )
  {
    oneStep(m_lastTime);
  }
}
