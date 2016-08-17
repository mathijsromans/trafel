#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <array>
#include <mutex>
#include <vector>

#include "body.h"

class Environment {

public:
  explicit Environment(unsigned int stepsize);
  ~Environment();
  void addBody(Body* body);
  void addSpaceship(Body* body);
  void init();
  const std::vector<Body*>& getBodies() const;
  void oneStep();
  std::array<double, 4> getStateDerivative(const std::array<double, 4>& x0, const Body* ignoreBody, unsigned int time) const;
  void boost(unsigned int spaceshipId, Body::Direction d);
  unsigned int getStepsize() const;
  unsigned int getCurrentTime() const;
  QPointF calcCentreOfMass(double time) const;

  static double calcOrbitalVelocity(double apogeeX, double eccentricity, double massCentralBody);

private:
  void preventCollisions() const;

private:
  static constexpr double gravitationalConstant=6.67408e-11;
  static const unsigned int timeAhead = 150;
  const unsigned int m_stepsize;
  unsigned int m_currentTime;
  std::vector<Body*> mo_bodies;
  std::vector<Body*> m_spaceships;
};

#endif /* ENVIRONMENT_H_ */
