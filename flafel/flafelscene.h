#ifndef FLAFELSCENE_H
#define FLAFELSCENE_H

#include "transformscene.h"
#include <array>
#include <memory>
#include <vector>
class Stopper;
class QTimer;
class PixmapItem;
class Ball;
class Player;

class FlafelScene : public TransformScene
{
  Q_OBJECT
public:
  explicit FlafelScene();
  ~FlafelScene() override;
  void stopperRemoved( const Stopper* stopper );
private:
  void init() override;
  void step() override;
  void eventClick(QPointF p, PointerEvent::Color c) override;
  void eventUnclick(QPointF p, PointerEvent::Color c) override;
  void eventMove(QPointF p, PointerEvent::Color c) override;
  bool showScore() const override { return true; }
  virtual unsigned int getFps() const { return ms_fps; }
  std::unique_ptr<Stopper> createStopper( QPointF start, QPointF end, QColor c );
  void addBalls();
  void advanceBalls();
  void putBallsInHoles();
  void removeOldBalls();
  void updateScores();
private:
  std::vector<std::unique_ptr<Ball>> m_balls;
  std::vector<std::unique_ptr<Player>> m_players;
  std::array<std::unique_ptr<Stopper>, 4> m_borderStoppers;
  std::vector<const Stopper*> m_allStoppers;
  double m_timeSinceLastNewBall;
  const unsigned int ms_fps = 20;
  const double ms_dt = 1.0 / ms_fps;
};

#endif // FLAFELSCENE_H
