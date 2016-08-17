#include "flafelscene.h"

#include "ball.h"
#include "pixmapitem.h"
#include "player.h"
#include "stopper.h"
#include "utilities.h"
#include <cassert>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <random>

namespace
{
  // game parameters
  double n_timeForNewBall = 1;
}

FlafelScene::FlafelScene()
 : TransformScene(),
   m_timeSinceLastNewBall(0)
{
}

FlafelScene::~FlafelScene()
{
}

void FlafelScene::init()
{
  if ( getNumPlayers() > static_cast<unsigned int>(PointerEvent::Color::MAX) )
  {
    throw std::logic_error("too many players: " + std::to_string(getNumPlayers()));
  }
  for ( unsigned int p = 0; p != getNumPlayers(); ++p )
  {
    m_players.push_back( std::make_unique<Player>(*this, p) );
    setScore(p, 10);
  }

  auto rect = getTableRect();
  m_borderStoppers[0] = createStopper( rect.topLeft(), rect.topRight(), Qt::white );
  m_borderStoppers[1] = createStopper( rect.topRight(), rect.bottomRight(), Qt::white );
  m_borderStoppers[2] = createStopper( rect.bottomRight(), rect.bottomLeft(), Qt::white );
  m_borderStoppers[3] = createStopper( rect.bottomLeft(), rect.topLeft(), Qt::white );
}

void FlafelScene::eventClick(QPointF p, PointerEvent::Color c)
{
  m_players[static_cast<unsigned int>(c)]->addStopper(createStopper(p, p, PointerEvent::getQColor(c)));
}

void FlafelScene::eventUnclick(QPointF /*p*/, PointerEvent::Color c)
{
  m_players[static_cast<unsigned int>(c)]->finishStopper();
}

void FlafelScene::eventMove(QPointF p, PointerEvent::Color c)
{
  m_players[static_cast<unsigned int>(c)]->extendStopper(p);
}

std::unique_ptr<Stopper> FlafelScene::createStopper(QPointF start, QPointF end, QColor c)
{
  QPen pen(c);
  pen.setWidthF(0.01*getTableRect().width());
  QGraphicsLineItem* line = addLine(QLineF(start, end), pen);
  auto stopper = std::make_unique<Stopper>(line);
  m_allStoppers.push_back(stopper.get());
  return stopper;
}

void FlafelScene::advanceBalls()
{
  if ( m_balls.empty() )
  {
    return;
  }
  std::vector<QLineF> lines;
  std::vector<QPointF> points;
  for ( const Stopper* stopper : m_allStoppers )
  {
    const QLineF line = stopper->item->line();
    QLineF transportLine = line.normalVector().unitVector();
    QPointF transport = 0.5 * Ball::getDiameter() * ( transportLine.p2() - transportLine.p1() );
    QLineF line1 = line.translated(+transport);
    QLineF line2 = line.translated(-transport);
    line2 = QLineF( line2.p2(), line2.p1() );
    lines.push_back( line1 );
    lines.push_back( line2 );
    points.push_back(line.p1());
    points.push_back(line.p2());
  }
  for ( auto&& b : m_balls )
  {
    b->advance(ms_dt, lines, points);
  }
}

void FlafelScene::putBallsInHoles()
{
  for ( auto&& b : m_balls )
  {
    for ( auto&& p : m_players )
    {
      p->putBallInHole(*b);
    }
  }

}

void FlafelScene::removeOldBalls()
{
  m_balls.erase(std::remove_if( m_balls.begin(),
                                m_balls.end(),
                                [](auto&& ball) { return ball->removeMe(); } ),
                m_balls.end() );
}

void FlafelScene::updateScores()
{
  for ( unsigned int p = 0; p != m_players.size(); ++p )
  {
    setScore(p, m_players[p]->getScore() );
  }
}

void FlafelScene::stopperRemoved(const Stopper* stopper)
{
  auto it = std::find(m_allStoppers.begin(), m_allStoppers.end(), stopper);
  assert(it != m_allStoppers.end());
  m_allStoppers.erase(it);
}

void FlafelScene::step()
{
  addBalls();
  advanceBalls();
  putBallsInHoles();
  removeOldBalls();
  updateScores();
}

void FlafelScene::addBalls()
{
  m_timeSinceLastNewBall += ms_dt;
  if ( m_timeSinceLastNewBall < n_timeForNewBall )
  {
    return;
  }
  m_balls.push_back( std::make_unique<Ball>(*this));
  m_timeSinceLastNewBall = 0;
}

