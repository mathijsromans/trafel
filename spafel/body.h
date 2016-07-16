#ifndef BODY_H_
#define BODY_H_

#include <QPointF>
#include <QLineF>
#include <array>
#include <deque>
#include <memory>
#include <iostream>
#include <vector>

class Drawable;
class Integrator;
class Environment;

class Body
{
public:

public:
  explicit Body(double x, double y, double vx, double vy, double mass, Environment* environment);
  ~Body();

  void oneStep(double stepsize, unsigned int time);

  double getMass() const;
  const std::array<double, 4>& getState(unsigned int time) const;

  unsigned int getId() const;


private:
  double random(double start, double end);
  void integrate(double stepsize, unsigned int time);

private:
  static unsigned int ms_nextUniqueId;
  unsigned int m_id;
  static const unsigned int ms_history = 200;
  std::array<std::array<double, 4>, ms_history> m_x;
  unsigned int m_lastTime;
  double m_mass;

  Environment* m_environment;
};

#endif /* BODY_H_ */
