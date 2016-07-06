#include "pointerevent.h"
#include <QTransform>

PointerEvent::PointerEvent(const std::array<QPoint, 3>& point)
  : m_points(point)
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

