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
  explicit Body(Environment* environment);
  ~Body();

  void oneStep(double stepsize);

  void setPosition(double m_x, double y);
  void setVelocity(double vxin, double vyin);

  void setMass(double mass);

  double getMass() const;
  const std::array<double, 4>& getState() const;
  const std::array<double, 2>& getParameters() const;

  void printPosition() const;
  void printVelocity() const;

  unsigned int getId() const;

  const Track& getTrack() const;
  void updateTrack();

private:
  double random(double start, double end);

private:
  static unsigned int ms_nextUniqueId;
  unsigned int m_id;
  std::array<double, 4> m_x;
  std::array<double, 2> m_para; // {mass, radius}

  std::unique_ptr<Integrator> m_integrator;
  Environment* m_environment;

  int m_nSteps;

  Track m_track;
};

#endif /* BODY_H_ */
