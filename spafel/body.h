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

  void oneStep(unsigned int time);

  double getMass() const;
  const std::array<double, 4>& getState(unsigned int time) const;

  enum class Direction { up, down, left, right };
  void boost(Direction d);


private:
  static const unsigned int ms_history = 200;
  std::array<std::array<double, 4>, ms_history> m_x;
  unsigned int m_lastTime;
  double m_mass;

  Environment* m_environment;
};

#endif /* BODY_H_ */
