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
  const std::vector<Body*>& getBodies() const;

  void oneStep(double tEnd, double stepsize);

  std::array<double, 4> getStateDerivative(const std::array<double, 4>& x0, Body* ignoreBody);

  double getFieldStrength(double x, double y);
  double getGravitationalConstant() const;

private:

private:
  const double m_gravitationalConstant;
  std::vector<Body*> mo_bodies;
  std::vector<Body*> mo_masslessBodies;
};

#endif /* ENVIRONMENT_H_ */
