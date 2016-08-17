#include "cargo.h"

#include "body.h"
#include "gravityscene.h"
#include "planet.h"
#include "spaceship.h"
#include "utilities.h"


#include <QPainter>

#include <cassert>

namespace
{
  const unsigned int lineWidth = 2;
}

Cargo::Cargo(Planet* origin, Planet* destination) :
  BodyItem(origin->getBody()),
  m_origin(origin),
  m_destination(destination),
  m_spaceship(0),
  m_status(Location::origin)
{
  assert(origin);
  assert(destination);
}

Cargo::Action Cargo::notifyCollision(Spaceship* spaceship, Planet* planet)
{
  if (m_status == Location::origin && m_origin == planet && !spaceship->hasCargo())
  {
    m_status = Location::spaceship;
    m_spaceship = spaceship;
    spaceship->setHasCargo(true);
    return Action::pickup;
  }
  else if (m_status == Location::spaceship && m_spaceship == spaceship && m_destination == planet)
  {
    m_status = Location::destination;
    spaceship->setHasCargo(false);
    return Action::dropoff;
  }
  return Action::none;
}

Body* Cargo::getCurrentBody() const
{
  switch (m_status)
  {
    case Location::origin :
      return m_origin->getBody();
    case Location::destination :
      return m_destination->getBody();
    case Location::spaceship :
      return m_spaceship->getBody();
  }
  assert(false);
  return 0;
}

QColor Cargo::getLineColor() const
{
  switch (m_status)
  {
    case Location::origin :
      return m_destination->getColor();
    case Location::destination :
      return Qt::white;
    case Location::spaceship :
      return Qt::white;
  }
  assert(false);
  return {};
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
  return Utilities::squareAt(0, 0, 6);
}

void Cargo::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem */*option*/,
                      QWidget */*widget*/)
{
  QPen pen(getLineColor());
  pen.setWidth(lineWidth);
  QBrush brush(m_destination->getColor());
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->drawRect(boundingRect());
}
