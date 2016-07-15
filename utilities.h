#ifndef UTILITIES_H
#define UTILITIES_H

#include <QPointF>
class QRectF;

namespace Utilities
{
QRectF squareAt( QPointF p, double size );
QRectF squareAt( double x, double y, double size );
double dist(QPointF p1, QPointF p2);
}

#endif // UTILITIES_H
