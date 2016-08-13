#include "ball.h"
#include "flafelscene.h"
#include "pixmapitem.h"
#include "player.h"
#include "stopper.h"
#include "utilities.h"

namespace
{
  const double totalStopperLength = 0.1;
  const double minStopperLength = 0.03;
}


std::array<std::unique_ptr<QPixmap>, 3> Player::ms_holes;

Player::Player(FlafelScene& scene, unsigned int player)
 : m_scene(scene),
   m_base(0),
   m_score(10),
   m_baseDiameter( 0.2 * scene.getTableRect().width() ),
   m_totalStopperLength(0)
{
  if ( !ms_holes.front() )
  {
    ms_holes = { std::make_unique<QPixmap>("../trafel/resources/hole_red.png"),
                 std::make_unique<QPixmap>("../trafel/resources/hole_green.png"),
                 std::make_unique<QPixmap>("../trafel/resources/hole_blue.png") };
    for ( auto&& p : ms_holes )
    {
      *p = p->scaled(QSize(m_baseDiameter,m_baseDiameter));
    }
  }
  QLineF playerLine = scene.getPlayerPosition(player);
  playerLine.setLength(0.5 * m_baseDiameter);
  m_base = new PixmapItem( *ms_holes[player%ms_holes.size()] );
  m_base->setPos(playerLine.p2());
  scene.addItem(m_base);
}

Player::~Player()
{
  delete m_base;
}

void Player::addStopper(std::unique_ptr<Stopper> stopper)
{
  m_totalStopperLength += stopper->length();
  m_stoppers.push_back(std::move(stopper));
  removeExcessStoppers();
}

void Player::extendStopper(QPointF newEnd)
{
  if ( m_stoppers.empty() )
  {
    return;
  }
  auto& stopper = m_stoppers.back();
  m_totalStopperLength -= stopper->length();
  stopper->setEnd(newEnd);
  m_totalStopperLength += stopper->length();
  removeExcessStoppers();
}

void Player::finishStopper()
{
  if ( !m_stoppers.empty() )
  {
    Stopper& stopper = *m_stoppers.back();
    double minLength = minStopperLength * m_scene.getTableRect().width();
    if ( stopper.length() < minLength )
    {
      QLineF line = stopper.item->line();
      if ( line.length() == 0 )
      {
        line.setP2( line.p1() + QPointF(minLength, 0) );
      }
      else
      {
        line.setLength(minLength);
      }
      extendStopper(line.p2());
    }
  }

}

void Player::putBallInHole(Ball& ball)
{
  if ( m_score > 0 &&
       !ball.isInBase() &&
       Utilities::dist(ball.getPos(), m_base->pos()) < 0.35 * m_baseDiameter )
  {
    ball.putInBase(m_base->pos());
    --m_score;
    if ( m_score == 0 )
    {
      m_base->hide();
    }
  }
}

unsigned int Player::getScore()
{
  return m_score;
}

void Player::removeExcessStoppers()
{
  const double maxLength = totalStopperLength * m_scene.getTableRect().width();
  while ( m_totalStopperLength > maxLength )
  {
    Stopper* stopper = m_stoppers.front().get();
    m_totalStopperLength -= stopper->length();
    m_stoppers.pop_front();
    m_scene.stopperRemoved(stopper);
  }
}
