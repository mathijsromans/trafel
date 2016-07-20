#include "bodyitem.h"

#include "body.h"
#include "gravityscene.h"

BodyItem::BodyItem(Body* body)
: QGraphicsItem(),
  m_body(body)
{
}

Body*
BodyItem::getBody() const
{
  return m_body;
}

void
BodyItem::update(const QRectF& tableRect, unsigned int time)
{
  const std::array<double, 4> state = m_body->getState(time);
  QPointF scenePos = GravityScene::envToScene(QPointF(state[0], state[1]), tableRect);
  setPos(scenePos.x(), scenePos.y());
}
