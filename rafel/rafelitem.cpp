#include "rafelitem.h"
#include "utilities.h"
#include <QPen>
#include <QPainter>

RafelItem::RafelItem(double radius , Type type) :
  QGraphicsItem(),
  m_radius(radius),
  m_type(type)
{
}

QRectF RafelItem::boundingRect() const
{
  return Utilities::squareAt(0, 0, m_radius);
}

void RafelItem::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem */*option*/,
                      QWidget */*widget*/)
{
  std::array<QColor, 3> colors = {Qt::red, Qt::green, Qt::blue };
  QColor color = colors[static_cast<unsigned int>(m_type)];
  QPen pen(color);
  QBrush brush(color);
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->drawEllipse(boundingRect());
}

RafelItem::Type RafelItem::getType() const
{
  return m_type;
}
