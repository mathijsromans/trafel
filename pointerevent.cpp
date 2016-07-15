#include "pointerevent.h"
#include "utilities.h"
#include <cmath>
#include <QTransform>

const std::array<QColor, 3> PointerEvent::ms_qcolors = { Qt::red, Qt::green, Qt::blue };

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

std::vector<PointerEvent::CPoint> PointerEvent::getPoints() const
{
  std::vector<CPoint> result;
  for (unsigned int i = 0; i != m_points.size(); ++i )
  {
    if ( !m_points[i].isNull() )
    {
      result.push_back( CPoint{static_cast<Color>(i), ms_qcolors[i], m_points[i]} );
    }
  }
  return result;
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
          Utilities::dist(m_points[i], other.m_points[i]) > 10 )
    {
      return true;
    }
  }
  return false;
}

