#include "mouseping.h"
#include "utilities.h"

MousePing::MousePing(QPointF p, double size)
  : QObject(),
    QGraphicsEllipseItem(),
    m_size(size)
{
}

double MousePing::getSize() const
{
  return m_size;
}

void MousePing::setSize(double size)
{
  m_size = size;
  setRect( Utilities::squareAt(rect().center(), m_size) );
}

