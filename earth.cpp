#include "earth.h"

#include <random>
#include "distributions.h"
#include <QDebug>

namespace
{
template <typename T>
T sqr(T x) { return x*x; }
}

Earth::Earth(double dx, double dy)
{
  std::random_device rd;
  std::mt19937 rng(rd());
  const double diag = sqrt(dx*dx+dy*dy);
  for ( unsigned int i = 0; i < 45; ++i )
  {
    Mountain m;
    m.x = dx * (-1+2.0*rng()/rng.max());
    m.y = dy * (-1+2.0*rng()/ rng.max());
    m.width = 0.3*diag*(0.2+0.8*rng()/rng.max());
    m.height = 30+50.0*rng()/rng.max();
    m_mountains.push_back(m);
  }
}

double Earth::height(double x, double y) const
{
  double result = 0;
  for ( auto&& m : m_mountains )
  {
    result += m.height * exp( -(sqr(m.x - x) + sqr(m.y - y)) / sqr(m.width));
  }
  return result;
}


