#include "flafelscene.h"

#include "ball.h"
#include "pixmapitem.h"
#include "player.h"
#include "stopper.h"
#include "utilities.h"
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <random>

namespace
{
  // game parameters
  double n_timeForNewBall = 1;

  // techical parameters
  double n_fps = 20;

  // derived parameters
  double n_dt = 1 / n_fps;
}

FlafelScene::FlafelScene()
 : TransformScene(),
   m_timer(new QTimer(this)),
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

  connect(m_timer, SIGNAL(timeout()), this, SLOT(step()));
  m_timer->start(1000/n_fps);
}

void FlafelScene::eventClick(QPointF p, PointerEvent::Color c)
{
  m_players[static_cast<unsigned int>(c)]->addStopper(createStopper(p, p, PointerEvent::getQColor(c)));
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
    QLineF line = stopper->item->line();
    QLineF transportLine = line.normalVector().unitVector();
    QPointF transport = 0.5 * Ball::getDiameter() * ( transportLine.p2() - transportLine.p1() );
    lines.push_back( line.translated(+transport) );
    lines.push_back( line.translated(-transport) );
    points.push_back(line.p1());
    points.push_back(line.p2());
  }
  for ( auto&& b : m_balls )
  {
    b->advance(n_dt, lines, points);
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
  m_timeSinceLastNewBall += n_dt;
  if ( m_timeSinceLastNewBall < n_timeForNewBall )
  {
    return;
  }
  m_balls.push_back( std::make_unique<Ball>(*this));
  m_timeSinceLastNewBall = 0;
}

