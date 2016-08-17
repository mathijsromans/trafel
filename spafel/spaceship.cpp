#include "body.h"
#include "gravityscene.h"
#include "spaceship.h"
#include <QPainter>

namespace
{
  const int width = 70;
}

std::unique_ptr<QPixmap> Spaceship::ms_redSpaceship;
std::unique_ptr<QPixmap> Spaceship::ms_greenSpaceship;
std::unique_ptr<QPixmap> Spaceship::ms_blueSpaceship;

Spaceship::Spaceship(Body* body, unsigned int id)
  : BodyItem(body),
    m_pixmap()
{
  if ( !ms_blueSpaceship )
  {
    initPixmaps();
  }
  switch (id%3)
  {
    case 0: m_pixmap = *ms_redSpaceship; break;
    case 1: m_pixmap = *ms_greenSpaceship; break;
    case 2: m_pixmap = *ms_blueSpaceship; break;
  }
  m_pixmap = m_pixmap.scaledToWidth(width);
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

void Spaceship::initPixmaps()
{
  ms_redSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_red.png");
  ms_greenSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_green.png");
  ms_blueSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_blue.png");
}

