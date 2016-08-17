#include "body.h"
#include "gravityscene.h"
#include "planet.h"
#include "utilities.h"
#include <QPainter>
#include <QPen>

namespace
{
  const double earthMass = 5.972e24;
  const double minRadius = 10.0;
}

Planet::Planet(Body* body, const QColor& color)
: BodyItem(body),
  m_radius(calcRadius(getBody()->getMass())),
  m_color(color)
{
}

QRectF Planet::boundingRect() const
{
  return Utilities::squareAt(0, 0, m_radius);
}

void Planet::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/,
                   QWidget */*widget*/)
{
  QPen pen(m_color);
  QBrush brush(m_color);
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->drawEllipse(boundingRect());
}

unsigned int
Planet::calcRadius(double mass)
{
  return static_cast<unsigned int>(std::max(std::pow(mass, 1/6.0) / std::pow(earthMass, 1/6.0) * 30.0, minRadius));
}

QColor Planet::getColor() const
{
  return m_color;
}
