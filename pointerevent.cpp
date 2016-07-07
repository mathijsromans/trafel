#include "pointerevent.h"
#include <cmath>
#include <QTransform>

namespace
{

template <typename T>
T sqr(T x) { return x*x; }

double dist(QPoint p1, QPoint p2)
{
  QPoint p = p1-p2;
  return sqrt(sqr(p.x()+sqr(p.y())));
}

}

PointerEvent::PointerEvent()
  : m_points()
{
  m_points.fill(QPoint(0,0));
}

PointerEvent::PointerEvent(const std::array<QPoint, 3>& points)
  : m_points(points)
{
}

QPoint PointerEvent::getAny() const
{
  for (QPoint p : m_points)
  {
    if ( p != QPoint(0,0) )
    {
      return p;
    }
  }
  return QPoint(0,0);
}

QPoint PointerEvent::getPoint(PointerEvent::Color c) const
{
  return m_points[static_cast<int>(c)];
}

void PointerEvent::transform(const QTransform& t)
{
  for (QPoint& p : m_points)
  {
    p = t.map(QPointF(p)).toPoint();
  }
}

bool PointerEvent::differsFrom(const PointerEvent& other) const
{
  for ( unsigned int i = 0; i != m_points.size(); ++i )
  {
    if ( m_points[i].isNull() != other.m_points[i].isNull() ||
         dist(m_points[i], other.m_points[i]) > 10 )
    {
      return true;
    }
  }
  return false;
}

