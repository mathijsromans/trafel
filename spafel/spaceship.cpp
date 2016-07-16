#include "body.h"
#include "gravityscene.h"
#include "spaceship.h"
#include <QDebug>
#include <QPainter>

std::unique_ptr<QPixmap> Spaceship::ms_blueSpaceship;

Spaceship::Spaceship(Body* body)
  : m_body(body)
{
  if ( !ms_blueSpaceship )
  {
    ms_blueSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_blue.png");
  }
  setPixmap(*ms_blueSpaceship);
  setOffset(-0.5*pixmap().rect().bottomRight());
}

void
Spaceship::update(const QRectF& tableRect)
{
//  const std::array<double, 4> state = m_body->getState();
//  QPointF scenePos = GravityScene::envToScene(QPointF(state[0], state[1]), tableRect);
//  setPos(scenePos);
}

void Spaceship::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* option,
                      QWidget* widget)
{
  painter->setPen(Qt::white);
  painter->drawRect(boundingRect());
  painter->drawPoint(boundingRect().center());
  QGraphicsPixmapItem::paint(painter, option, widget);
}

