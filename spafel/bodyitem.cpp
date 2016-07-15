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
: QGraphicsEllipseItem(),
  m_body(body),
  m_radius(1.0)
{
  m_radius = calcRadius(body->getMass());
  setRect(0, 0, 2*m_radius, 2*m_radius);
  setPen(QPen(color));
  setBrush(QBrush(color));
}


Body*
BodyItem::getBody() const
{
  return m_body;
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
  setPos(scenePos.x() - m_radius, scenePos.y() - m_radius);
}
