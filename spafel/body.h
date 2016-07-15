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
  struct Track
  {
    std::deque<QPointF> points;
    std::deque<QLineF> lines;
  };

public:
  explicit Body(double x, double y, double vx, double vy, double mass, Environment* environment);
  ~Body();

  void oneStep(double stepsize, unsigned int time);

  double getMass() const;
  const std::array<double, 4>& getState(unsigned int time) const;
  const std::array<double, 2>& getParameters() const;

  unsigned int getId() const;

  const Track& getTrack() const;
  void updateTrack();

private:
  double random(double start, double end);
  void integrate(double stepsize, unsigned int time);

private:
  static unsigned int ms_nextUniqueId;
  unsigned int m_id;
  std::vector<std::array<double, 4>> m_x;
  std::array<double, 2> m_para; // {mass, radius}

  Environment* m_environment;

  Track m_track;
};

#endif /* BODY_H_ */
