#include "body.h"
#include "gravityscene.h"
#include "planet.h"
#include "utilities.h"
#include <QPainter>
#include <QPen>

namespace
{
  const double earthMass = 5.972e24;
}

Planet::Planet(Body* body, const QColor& lineColor)
: BodyItem(body),
  m_radius(calcRadius(getBody()->getMass())),
  m_lineColor(lineColor)
{
}

QRectF Planet::boundingRect() const
{
  return Utilities::squareAt(0, 0, m_radius);
}

void Planet::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/,
                   QWidget */*widget*/)
{
  QColor color = getBody()->getColor();
  QPen pen(m_lineColor);
  pen.setWidth(4);
  QBrush brush(color);
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->drawEllipse(boundingRect());
}

unsigned int
Planet::calcRadius(double mass)
{
  return std::pow(mass, 1/6.0) / std::pow(earthMass, 1/6.0) * 20.0;
}

QColor Planet::getLineColor()
{
  return m_lineColor;
}
