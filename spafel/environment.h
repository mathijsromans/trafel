#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <array>
#include <mutex>
#include <vector>

#include "body.h"

class Environment {

public:
  explicit Environment(double gravitationalConstant=6.67408e-11);
  ~Environment();

  void addBody(Body* body);
  void addMasslessBody(Body* body);
  void removeBody(Body* body);
  void clearAllBodies();

  void oneStep(double tEnd, double stepsize);
//  void oneStepThread(double tEnd, double stepsize);

  std::array<double, 4> getStateDerivative(const std::array<double, 4>& x0, Body* ignoreBody);

  double getFieldStrength(double x, double y);
  double getGravitationalConstant() const;

private:
  void oneStepImpl(double tEnd, double stepsize);

private:
  const double m_gravitationalConstant;
  std::vector<Body*> m_bodies;
  std::vector<Body*> m_masslessBodies;

  std::array<double, 4> m_x1;
  std::array<double, 4> m_x2;

  std::array<double, 2> m_para1;
  std::array<double, 2> m_para2;

  std::array<double, 4> m_stateDerivative;

  mutable std::mutex m_mutex;
};

#endif /* ENVIRONMENT_H_ */
