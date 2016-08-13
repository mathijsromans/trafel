#include "ball.h"
#include "flafelscene.h"
#include "pixmapitem.h"
#include "player.h"
#include "stopper.h"
#include "utilities.h"

std::array<std::unique_ptr<QPixmap>, 3> Player::ms_holes;

Player::Player(FlafelScene& scene, unsigned int player)
 : m_base(0),
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
  m_stoppers.push_back(std::move(stopper));
}

void Player::extendStopper(QPointF newEnd)
{
  auto& stopper = m_stoppers.back();
  QLineF line = stopper->item->line();
  line.setP2(newEnd);
  stopper->item->setLine(line);
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
