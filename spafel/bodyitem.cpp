#include "bodyitem.h"

#include "body.h"
#include "gravityscene.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QPen>

namespace
{
  const double AU = 149.6e9;
  const double earthMass = 5.972e24;
}

BodyItem::BodyItem(Body* body, const QColor& color)
: m_body(body),
  m_item(0),
  m_radius(1.0)
{
  m_radius = calcRadius(body->getMass());
  std::cout << "radius: " << m_radius << std::endl;
  m_item = new QGraphicsEllipseItem(0, 0, 2*m_radius, 2*m_radius);
  m_item->setPen(QPen(color));
  m_item->setBrush(QBrush(color));
}


QGraphicsEllipseItem*
BodyItem::getItem() const
{
  return m_item;
}


Body*
BodyItem::getBody() const
{
  return m_body.get();
}


unsigned int
BodyItem::getRadius() const
{
  return m_radius;
}


unsigned int
BodyItem::calcRadius(double mass)
{
  return std::pow(mass, 1/6.0) / std::pow(earthMass, 1/6.0) * 2.0;
}


void
BodyItem::update(const QRectF& tableRect)
{
  const std::array<double, 4> state = m_body->getState();
  QPointF scenePos = GravityScene::envToScene(QPointF(state[0], state[1]), tableRect);
  m_item->setPos(scenePos.x() - m_radius, scenePos.y() - m_radius);
}
