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
std::unique_ptr<QPixmap> Spaceship::ms_redSpaceshipRocketOff;
std::unique_ptr<QPixmap> Spaceship::ms_greenSpaceshipRocketOff;
std::unique_ptr<QPixmap> Spaceship::ms_blueSpaceshipRocketOff;

Spaceship::Spaceship(Body* body, unsigned int id)
  : BodyItem(body),
    m_pixmap(),
    m_pixmapRocketOff(),
    m_playerId(id),
    m_rocketOn(0)
{
  if ( !ms_blueSpaceship )
  {
    initPixmaps();
  }
  switch (id%3)
  {
    case 0: m_pixmap = *ms_redSpaceship; m_pixmapRocketOff = *ms_redSpaceshipRocketOff; break;
    case 1: m_pixmap = *ms_greenSpaceship; m_pixmapRocketOff = *ms_greenSpaceshipRocketOff; break;
    case 2: m_pixmap = *ms_blueSpaceship; m_pixmapRocketOff = *ms_blueSpaceshipRocketOff; break;
  }
  m_pixmap = m_pixmap.scaledToWidth(width);
  m_pixmapRocketOff = m_pixmapRocketOff.scaledToWidth(width);
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
  if (m_rocketOn > 0)
  {
    painter->drawPixmap(boundingRect().topLeft(), m_pixmap);
    m_rocketOn--;
  }
  else
  {
    painter->drawPixmap(boundingRect().topLeft(), m_pixmapRocketOff);
  }
}

unsigned int Spaceship::getPlayerId() const
{
  return m_playerId;
}

void Spaceship::setShowRocketOn()
{
  m_rocketOn += 10;
}

void Spaceship::initPixmaps()
{
  ms_redSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_red.png");
  ms_greenSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_green.png");
  ms_blueSpaceship = std::make_unique<QPixmap>("../trafel/resources/rocket_blue.png");
  ms_redSpaceshipRocketOff = std::make_unique<QPixmap>("../trafel/resources/rocket_red_noburn.png");
  ms_greenSpaceshipRocketOff = std::make_unique<QPixmap>("../trafel/resources/rocket_green_noburn.png");
  ms_blueSpaceshipRocketOff = std::make_unique<QPixmap>("../trafel/resources/rocket_blue_noburn.png");
}

