#include "bodyitem.h"

#include "body.h"
#include "gravityscene.h"
#include <QDebug>
#include <cassert>

BodyItem::BodyItem(Body* body)
: QGraphicsItem(),
  m_body(body)
{
  assert(m_body);
}

Body* BodyItem::getBody() const
{
  return m_body;
}

void BodyItem::setBody(Body* body)
{
  m_body = body;
}

void BodyItem::updateItem(const QRectF& tableRect, QPointF centreOfMass)
{
  const std::array<double, 4> state = m_body->getCurrentState();
  QPointF scenePos = GravityScene::envToScene(QPointF(state[0], state[1]), tableRect, centreOfMass);
  setPos(scenePos.x(), scenePos.y());
  setRotation(180*atan2(state[3], state[2])/M_PI);
}
