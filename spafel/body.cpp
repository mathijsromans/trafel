#include "environment.h"

#include "body.h"
#include <cassert>

namespace
{
  const double integrationTolerance = 10.0;
  const unsigned int maxSteps = 200;
}


Body::Body(double x, double y, double vx, double vy, double mass, QColor trackColor, Environment* environment)
: m_x(),
  m_trackChanged(true),
  m_currentTime(0),
  m_mass(mass),
  m_trackColor(trackColor),
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

bool
Body::calculate(unsigned int time)
{
  assert(time > m_currentTime);
  assert(time < m_currentTime + m_x.size());
  double stepsize = m_environment->getStepsize();

  IntegrationResult result;
  while (!result.success)
  {
    result = integrateRK45(stepsize, time);

    stepsize = 0.9 * stepsize * integrationTolerance / result.error;  // new stepsize based on estimated integration error
    if (std::ceil(m_environment->getStepsize()/stepsize) > maxSteps)
    {
      stepsize = m_environment->getStepsize()/(maxSteps+1);
    }
  }

  assert(result.success);
  m_x[time%m_x.size()] = result.x;
  return true;
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

void
Body::setState(const std::array<double, 4>& x, unsigned int time)
{
  assert(time >= m_currentTime);
  assert(time < m_currentTime+m_x.size());
  m_x[time%m_x.size()] = x;
}

void Body::boost(Body::Direction d)
{
  const double power = 0.05;
  const double change = 1.0 + power;
  double& vx = m_x[m_currentTime%m_x.size()][2];
  double& vy = m_x[m_currentTime%m_x.size()][3];
  switch (d)
  {
    case Direction::up:  vx *= change; vy *= change; break;
    case Direction::down:vx /= change; vy /= change; break;
    case Direction::left:  std::tie(vx, vy) = std::make_pair(vx + power * vy, vy - power * vx); break;
    case Direction::right: std::tie(vx, vy) = std::make_pair(vx - power * vy, vy + power * vx); break;
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

QColor Body::getTrackColor() const
{
  return m_trackColor;
}

Body::IntegrationResult Body::integrateRK45(double stepsize, unsigned int time) const
{
  // RKF45 ODE integration: https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta%E2%80%93Fehlberg_method
  double timeDone = 0.0;
  const std::size_t dim = 4;
  std::array<double, dim> x = getState(time-1);

  std::array<double, dim> x0k1, x0k2, x0k3, x0k4, x0k5;
  std::array<double, dim> k1, k2, k3, k4, k5, k6;
  std::array<double, dim> xRK5;

  std::array<std::array<double, 7>, 7> a;  // first row and column are not user, to keep in line with mathematical notation starting at 1
  a[2][1] = 1.0 / 4.0;
  a[3][1] = 3.0 / 32.0;
  a[3][2] = 9.0 / 32.0;
  a[4][1] = 1932.0 / 2197.0;
  a[4][2] = -7200.0 / 2197.0;
  a[4][3] = 7296.0 / 2197.0;
  a[5][1] = 439.0 / 216.0;
  a[5][2] = -8.0;
  a[5][3] = 3680.0 / 513.0;
  a[5][4] = -845.0 / 4104.0;
  a[6][1] = -8.0 / 27.0;
  a[6][2] = 2.0;
  a[6][3] = -3544.0 / 2565.0;
  a[6][4] = 1859.0 / 4104.0;
  a[6][5] = -11.0 / 40.0;

  std::array<double, 7> b5;
  b5[0] = 0.0;
  b5[1] = 16.0 / 135.0;
  b5[2] = 0.0;
  b5[3] = 6656.0 / 12825.0;
  b5[4] = 28561.0 / 56430.0;
  b5[5] = -9.0 / 50.0;
  b5[6] = 2.0 / 55.0;

  std::array<double, 6> b4;
  b4[0] = 0.0;
  b4[1] = 25.0 / 216.0;
  b4[2] = 0.0;
  b4[3] = 1408.0 / 2565.0;
  b4[4] = 2197.0 / 4104;
  b4[5] = -1.0 / 5.0;

  IntegrationResult result;
  unsigned int nSteps = std::ceil(m_environment->getStepsize()/stepsize);

  while (timeDone < m_environment->getStepsize())
  {
    if (timeDone+stepsize > m_environment->getStepsize())
    {
      stepsize = m_environment->getStepsize()-timeDone;
    }

    std::array<double, dim> force = m_environment->getStateDerivative(x, this, time-1);
    for (std::size_t j = 0; j < dim; j++)
    {
      k1[j] = force[j];
      x0k1[j] = x[j] + a[2][1] * k1[j];
    }

    force = m_environment->getStateDerivative(x0k1, this, time-1);
    for (std::size_t j = 0; j < dim; j++)
    {
      k2[j] = force[j];
      x0k2[j] = x[j] + stepsize * (a[3][1]*k1[j] + a[3][2]*k2[j]);
    }

    force = m_environment->getStateDerivative(x0k2, this, time-1);
    for (std::size_t j = 0; j < dim; j++)
    {
      k3[j] = force[j];
      x0k3[j] = x[j] + stepsize * (a[4][1]*k1[j] + a[4][2]*k2[j] + a[4][3]*k3[j]);
    }

    force = m_environment->getStateDerivative(x0k3, this, time-1);
    for (std::size_t j = 0; j < dim; j++)
    {
      k4[j] = force[j];
      x0k4[j] = x[j] + stepsize * (a[5][1]*k1[j] + a[5][2]*k2[j] + a[5][3]*k3[j] + a[5][4]*k4[j]);
    }

    force = m_environment->getStateDerivative(x0k4, this, time-1);
    for (std::size_t j = 0; j < dim; j++)
    {
      k5[j] = force[j];
      x0k5[j] = x[j] + stepsize * (a[6][1]*k1[j] + a[6][2]*k2[j] + a[6][3]*k3[j] + a[6][4]*k4[j] + a[6][5]*k5[j]);
    }

    force = m_environment->getStateDerivative(x0k5, this, time-1);
    for (std::size_t j = 0; j < dim; j++)
    {
      k6[j] = force[j];
    }

    std::array<double, dim> xRK4;
    std::array<double, dim> error;
    for (std::size_t i = 0; i < dim; i++)
    {
      xRK5[i] = x[i] + stepsize * (b5[1]*k1[i] + b5[3]*k3[i] + b5[4]*k4[i] + b5[5]*k5[i] + b5[6]*k6[i]);
      xRK4[i] = x[i] + stepsize * (b4[1]*k1[i] + b4[3]*k3[i] + b4[4]*k4[i] + b4[5]*k5[i]);
      error[i] = std::fabs(xRK5[i]-xRK4[i]);
    }

    x = xRK5;
    timeDone += stepsize;

    result.error = std::sqrt(error[0] + error[1]);
    if (result.error > integrationTolerance && nSteps < maxSteps)
    {
      result.success = false;
      result.x.fill(0);
      result.nSteps = nSteps;
      return result;
    }
  }

  result.success = true;
  result.x = x;
  result.nSteps = nSteps;
  return result;
}
