#include "mouseping.h"
#include "utilities.h"
#include <QPen>

MousePing::MousePing(QPointF p)
  : QObject(),
    QGraphicsEllipseItem( Utilities::squareAt(p, 0) ),
    m_size(0),
    m_animation(this, "size")
{
  QPen pen;
  pen.setWidth(3);
  pen.setColor(Qt::blue);
  setPen(pen);

  m_animation.setDuration(100);
  m_animation.setStartValue(5);
  m_animation.setEndValue(55);
  m_animation.start();
  connect( &m_animation, SIGNAL(finished()), this, SLOT(animationFinished()) );
}

MousePing::~MousePing()
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

void MousePing::animationFinished()
{
  deleteLater();  // is this okay?
}

