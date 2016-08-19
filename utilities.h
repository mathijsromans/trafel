#ifndef UTILITIES_H
#define UTILITIES_H

#include <QPointF>
class QRectF;

namespace Utilities
{

QRectF squareAt( QPointF p, double size );
QRectF squareAt( double x, double y, double size );
double dist(QPointF p1, QPointF p2);
double dist2(QPointF p1, QPointF p2);

template <typename T> int sign(T val)
{
  return (T(0) < val) - (val < T(0));
}

template <typename T>
T sqr(T x) { return x*x; }

template <typename T1, typename T2>
bool contains( const T1& vec, const T2& elem )
{
  return std::find( vec.begin(), vec.end(), elem ) != vec.end();
}

unsigned int getRnd( unsigned int min, unsigned int max );

}

#endif // UTILITIES_H
