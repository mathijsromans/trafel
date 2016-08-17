#include "cargoitem.h"

#include "body.h"
#include "gravityscene.h"
#include "planet.h"
#include "spaceship.h"
#include "utilities.h"


#include <QPainter>

#include <cassert>


CargoItem::CargoItem(Planet* origin, Planet* destination) :
  BodyItem(origin->getBody()),
  m_origin(origin),
  m_destination(destination),
  m_spaceship(0),
  m_status(Status::origin)
{
  assert(origin);
  assert(destination);
}

void CargoItem::setSpaceship(Spaceship* spaceship)
{
  assert(m_status == Status::origin);
  m_status = Status::spaceship;
  m_spaceship = spaceship;
}

Body* CargoItem::getCurrentBody() const
{
  switch (m_status)
  {
    case Status::origin :
      return m_origin->getBody();
    case Status::destination :
      return m_destination->getBody();
    case Status::spaceship :
      return m_spaceship->getBody();
  }
  assert(false);
  return 0;
}

QColor CargoItem::getColor() const
{
  switch (m_status)
  {
  case Status::origin :
    return Qt::red;
  case Status::destination :
    return Qt::green;
  case Status::spaceship :
    return m_spaceship->getBody()->getColor();
  }
  assert(false);
  return {};
}

void CargoItem::updateItem(const QRectF &tableRect, QPointF centreOfMass)
{
  Body* body = getCurrentBody();
  assert(body);
  const std::array<double, 4>& state = body->getCurrentState();
  QPointF scenePos = GravityScene::envToScene(QPointF(state[0], state[1]), tableRect, centreOfMass);
  setPos(scenePos.x(), scenePos.y());
}

QRectF CargoItem::boundingRect() const
{
  return Utilities::squareAt(0, 0, 10);
}

void CargoItem::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem */*option*/,
                      QWidget */*widget*/)
{
  QColor color = getColor();
  QPen pen(Qt::white);
  QBrush brush(color);
  painter->setPen(pen);
  painter->setBrush(brush);
  QRectF(0.0, 0.0, 10, 10);
  painter->drawRect(boundingRect());
}
