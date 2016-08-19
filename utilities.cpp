#include "utilities.h"

#include <QRectF>

namespace Utilities
{

QRectF squareAt(QPointF p, double size)
{
  return QRectF(p.x()-0.5*size, p.y()-0.5*size, size, size);
}

QRectF squareAt(double x, double y, double size)
{
  return QRectF(x-0.5*size, y-0.5*size, size, size);
}

double dist(QPointF p1, QPointF p2)
{
  return sqrt(dist2(p1, p2));
}

double dist2(QPointF p1, QPointF p2)
{
  QPointF p = p1-p2;
  return sqr(p.x())+sqr(p.y());
}

unsigned int getRnd( unsigned int min, unsigned int max )
{
  static std::random_device rd;
  static std::mt19937 rng(rd());
  std::uniform_int_distribution<> distribution(min, max);
  return distribution(rng);
}

}
