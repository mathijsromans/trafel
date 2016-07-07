#ifndef UTILITIES_H
#define UTILITIES_H

#include <QPointF>
class QRectF;

namespace Utilities
{
QRectF squareAt( QPointF p, double size );
QRectF squareAt( double x, double y, double size );
}

#endif // UTILITIES_H
