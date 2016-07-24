#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <array>
#include <mutex>
#include <vector>

#include "body.h"

class Environment {

public:
  explicit Environment(unsigned int stepsize, double gravitationalConstant=6.67408e-11);
  ~Environment();
  void addBody(Body* body);
  void addSpaceship(Body* body);
  const std::vector<Body*>& getBodies() const;
  void oneStep(unsigned int time);
  std::array<double, 4> getStateDerivative(const std::array<double, 4>& x0, Body* ignoreBody, unsigned int time);
  void boost(unsigned int spaceshipId, Body::Direction d);
  unsigned int getStepsize() const;

private:
  static const unsigned int timeAhead = 200;
  const unsigned int m_stepsize;
  const double m_gravitationalConstant;
  unsigned int m_lastTime;
  std::vector<Body*> mo_bodies;
  std::vector<Body*> m_spaceships;
};

#endif /* ENVIRONMENT_H_ */
