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
  const std::vector<Body*>& getBodies() const;
  void oneStep(double stepsize, unsigned int time);
  std::array<double, 4> getStateDerivative(const std::array<double, 4>& x0, Body* ignoreBody, unsigned int time);

private:
  static const unsigned int timeAhead = 100;
  const double m_gravitationalConstant;
  unsigned int m_lastTime;
  std::vector<Body*> mo_bodies;
};

#endif /* ENVIRONMENT_H_ */
