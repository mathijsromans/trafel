#include "pointerevent.h"
#include <cmath>
#include <QTransform>

namespace
{

template <typename T>
T sqr(T x) { return x*x; }

double dist(QPointF p1, QPointF p2)
{
  QPointF p = p1-p2;
  return sqrt(sqr(p.x()+sqr(p.y())));
}

}

PointerEvent::PointerEvent()
  : m_points()
{
  m_points.fill(QPointF(0,0));
}

PointerEvent::PointerEvent(const std::array<QPointF, 3>& points)
  : m_points(points)
{
}

QPointF PointerEvent::getAny() const
{
  for (QPointF p : m_points)
  {
    if ( !p.isNull() )
    {
      return p;
    }
  }
  return QPointF(0,0);
}

QPointF PointerEvent::getPoint(PointerEvent::Color c) const
{
  return m_points[static_cast<int>(c)];
}

void PointerEvent::transform(const QTransform& t)
{
  for (QPointF& p : m_points)
  {
    if ( !p.isNull() )
    {
      p = t.map(p);
    }
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

