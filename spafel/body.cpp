#include "environment.h"

#include "body.h"
#include <cassert>

unsigned int Body::ms_nextUniqueId = 1;

Body::Body(double x, double y, double vx, double vy, double mass, Environment* environment)
: m_id(ms_nextUniqueId++),
  m_x(1),
  m_para(),
  m_environment(environment),
  m_track()
{
  m_x[0][0] = x;
  m_x[0][1] = y;
  m_x[0][2] = vx;
  m_x[0][3] = vy;

  m_para[0] = mass; // mass [kg]
  m_para[1] = 15.0; // radius [m]
}


Body::~Body()
{
}


void
Body::oneStep(double stepsize, unsigned int time)
{
  integrate(stepsize, time);
}


double
Body::getMass() const
{
  return m_para[0];
}


const std::array<double, 4>&
Body::getState(unsigned int time) const
{
  assert(time<m_x.size());
  return m_x[time];
}


const std::array<double, 2>&
Body::getParameters() const
{
  return m_para;
}


unsigned int
Body::getId() const
{
  return m_id;
}


const Body::Track&
Body::getTrack() const
{
  return m_track;
}


double
Body::random(double start, double end)
{
  double randomDouble = start+(end-start)*rand()/(RAND_MAX + 1.0);
  return randomDouble;
}


void
Body::updateTrack()
{
  m_track.points.clear();
  for ( auto&& x : m_x )
  {
    m_track.points.push_back(QPointF(x[0], x[1]));
  }
}

void
Body::integrate(double stepsize, unsigned int time)
{
  assert(time == m_x.size()-1);
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

  m_x.push_back({});
  for (std::size_t i = 0; i < 4; i++)
  {
    m_x.back()[i] = x[i] + 1.0/6.0 * (k1[i]+2*k2[i]+2*k3[i]+k4[i]);
  }

}
