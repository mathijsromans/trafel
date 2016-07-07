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

}