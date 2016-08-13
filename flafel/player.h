#ifndef PLAYER_H
#define PLAYER_H

class Ball;
class FlafelScene;
class PixmapItem;
class Stopper;
#include "nocopy.h"
#include <QPixmap>
#include <array>
#include <deque>
#include <memory>

class Player : NoCopy
{
public:
  explicit Player(FlafelScene& scene, unsigned int player);
  ~Player();
  void addStopper( std::unique_ptr<Stopper> stopper );
  void extendStopper( QPointF newEnd );
  void finishStopper();
  void putBallInHole( Ball& ball );
  unsigned int getScore();
private:
  void removeExcessStoppers();
private:
  FlafelScene& m_scene;
  PixmapItem* m_base;
  unsigned int m_score;
  double m_baseDiameter;
  double m_totalStopperLength;
  std::deque<std::unique_ptr<Stopper>> m_stoppers;
  static std::array<std::unique_ptr<QPixmap>, 3> ms_holes;

};

#endif // PLAYER_H
