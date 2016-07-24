#ifndef BODY_H_
#define BODY_H_

#include <QColor>
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
  explicit Body(double x, double y, double vx, double vy, double mass, QColor color, Environment* environment);
  ~Body();

  void oneStep();
  void calculate(unsigned int time);

  double getMass() const;
  const std::array<double, 4>& getCurrentState() const;
  const std::array<double, 4>& getState(unsigned int time) const;

  enum class Direction { up, down, left, right };
  void boost(Direction d);
  bool trackChanged();

  static const unsigned int timeAhead = 200;
  QColor getColor() const;

private:
  std::array<std::array<double, 4>, timeAhead> m_x; // contains states from m_currentTime to m_currentTime + m_x.size()
  bool m_trackChanged;
  unsigned int m_currentTime;
  double m_mass;
  QColor m_color;

  Environment* m_environment;
};

#endif /* BODY_H_ */
