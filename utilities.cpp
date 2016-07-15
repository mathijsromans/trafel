#include "utilities.h"

#include <QRectF>

namespace Utilities
{

template <typename T>
T sqr(T x) { return x*x; }

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
  QPointF p = p1-p2;
  return sqrt(sqr(p.x()+sqr(p.y())));
}

}
