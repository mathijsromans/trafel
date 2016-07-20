#include "body.h"
#include "gravityscene.h"
#include "spaceship.h"
#include <QPainter>

std::unique_ptr<QPixmap> Spaceship::ms_blueSpaceship;

Spaceship::Spaceship(Body* body)
  : BodyItem(body),
    m_pixmap()
{
  if ( !ms_blueSpaceship )
  {
    ms_blueSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_blue.png");
  }
  m_pixmap = *ms_blueSpaceship;
}

QRectF Spaceship::boundingRect() const
{
  QSize s = m_pixmap.size();
  return QRectF(-0.5*s.width(), -0.5*s.height(), s.width(), s.height());
}

void Spaceship::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* /*option*/,
                      QWidget* /*widget*/)
{
  painter->drawPixmap(boundingRect().topLeft(), m_pixmap);
}

