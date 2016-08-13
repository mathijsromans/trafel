#include "pointerevent.h"
#include "utilities.h"
#include <cmath>
#include <QTransform>

const std::array<QColor, 3> PointerEvent::ms_qcolors = { Qt::red, Qt::green, Qt::blue };

PointerEvent::PointerEvent()
  : m_points(),
    m_ping()
{
  m_points.fill(QPointF(0,0));
  m_ping.fill(false);
}

PointerEvent::PointerEvent(const std::array<QPointF, 3>& points)
  : m_points(points)
{
  m_ping.fill(false);
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
  return m_points[static_cast<unsigned int>(c)];
}

std::vector<PointerEvent::CPoint> PointerEvent::getPoints() const
{
  std::vector<CPoint> result;
  for (unsigned int i = 0; i != m_points.size(); ++i )
  {
    if ( !m_points[i].isNull() )
    {
      result.push_back( CPoint{static_cast<Color>(i), m_points[i], m_ping[i]} );
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

bool PointerEvent::compareTo(const PointerEvent& other)
{
  bool result = false;
  for ( unsigned int i = 0; i != m_points.size(); ++i )
  {
    if ( m_points[i].isNull() != other.m_points[i].isNull() )
    {
      m_ping[i] = true;
      result = true;
    }
    if ( Utilities::dist(m_points[i], other.m_points[i]) > 10 )
    {
      result = true;
    }
  }
  return result;
}

QColor PointerEvent::getQColor(PointerEvent::Color color)
{
  return ms_qcolors[static_cast<unsigned int>(color)];

}

