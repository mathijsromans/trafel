#include "cargo.h"

#include "body.h"
#include "gravityscene.h"
#include "planet.h"
#include "spaceship.h"
#include "utilities.h"


#include <QPainter>

#include <cassert>


Cargo::Cargo(Planet* origin, Planet* destination) :
  BodyItem(origin->getBody()),
  m_origin(origin),
  m_destination(destination),
  m_spaceship(0),
  m_status(Status::origin)
{
  assert(origin);
  assert(destination);
}

void Cargo::notifyCollision(Spaceship* spaceship, Planet* planet)
{
  if (m_status == Status::origin && m_origin == planet)
  {
    m_status = Status::spaceship;
    m_spaceship = spaceship;
  }
  else if (m_status == Status::spaceship && m_spaceship == spaceship && m_destination == planet)
  {
    m_status = Status::destination;
  }
}

Body* Cargo::getCurrentBody() const
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

void Cargo::updateItem(const QRectF &tableRect, QPointF centreOfMass)
{
  Body* body = getCurrentBody();
  assert(body);
  const std::array<double, 4>& state = body->getCurrentState();
  QPointF scenePos = GravityScene::envToScene(QPointF(state[0], state[1]), tableRect, centreOfMass);
  setPos(scenePos.x(), scenePos.y());
}

QRectF Cargo::boundingRect() const
{
  return Utilities::squareAt(0, 0, 10);
}

void Cargo::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem */*option*/,
                      QWidget */*widget*/)
{
  QPen pen(Qt::white);
  QBrush brush(m_destination->getLineColor());
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->drawRect(boundingRect());
}
