#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>

#include "environment.h"

/**
 * RK4 (Runge Kutta 4th order) fixed stepsize integrator class
 * The most basic and conventional higher order RK4 ODE integrator.
 * A RK4 object is composed of a EOMrk equations of motion object.
 * A pointer to this EOMrk object is passed as parameter.
 */
class Integrator
{
public:
  explicit Integrator(Environment* environment, Body* body);
  ~Integrator();

  const std::array<double, 4>& integrate(double stepsize);

private:
  void oneStep(double stepsize);

private:
  Environment* m_environment;
  Body* m_body;

  std::array<double, 4> x;

  std::array<double, 4> px0;
  std::array<double, 4> px1;

  std::array<double, 4> x0k1;
  std::array<double, 4> x0k2;
  std::array<double, 4> x0k3;

  std::array<double, 4> k1;
  std::array<double, 4> k2;
  std::array<double, 4> k3;
  std::array<double, 4> k4;
};

#endif /*INTEGRATOR_H_*/
