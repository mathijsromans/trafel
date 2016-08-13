#ifndef PLAYER_H
#define PLAYER_H

class FlafelScene;
class PixmapItem;
class Stopper;
#include "nocopy.h"
#include <QPixmap>
#include <array>
#include <memory>
#include <vector>

class Player : NoCopy
{
public:
  explicit Player(FlafelScene& scene, unsigned int player);
  ~Player();
  void addStopper( std::unique_ptr<Stopper> stopper );
  void extendStopper( QPointF newEnd );
  void putBallInHole( Ball& ball );
  unsigned int getScore();
private:
  PixmapItem* m_base;
  unsigned int m_score;
  double m_baseDiameter;
  double m_totalStopperLength;
  std::vector<std::unique_ptr<Stopper>> m_stoppers;
  static std::array<std::unique_ptr<QPixmap>, 3> ms_holes;

};

#endif // PLAYER_H
