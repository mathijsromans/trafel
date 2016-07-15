#ifndef BODY_H_
#define BODY_H_

#include <array>
#include <memory>
#include <iostream>
#include <vector>

class Drawable;
class Integrator;
class Environment;

class Body
{
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
};

#endif /* BODY_H_ */
