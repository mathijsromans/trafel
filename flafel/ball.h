#ifndef BALL_H
#define BALL_H

#include "nocopy.h"
#include <QPointF>
#include <QLineF>
#include <memory>
class PixmapItem;
class QPixmap;
class FlafelScene;
class Stopper;

class Ball : NoCopy
{
public:
  explicit Ball(FlafelScene& scene);
  virtual ~Ball();
  void advance( double dt, const std::vector<QLineF>& stopLines, const std::vector<QPointF>& stopPoints, QLineF ignoreLine = QLineF(), QPointF ignorePoint = QPointF() );
  QPointF getPos() const;
  void setPos(QPointF pos);
  bool isInBase() const;
  void putInBase(QPointF pos);
  bool removeMe() const;
  static double getDiameter();

private:
  QPointF circleLineSegmentIntersection(QLineF line, QPointF center, double radius);

private:
  PixmapItem* m_item;
  QPointF m_v;
  bool m_isInBase = false;
  double m_timeInBase = 0;
  static constexpr double ms_timeBallsRemainsInBase = 2;
  static constexpr double ms_speed = 0.35;
  static std::unique_ptr<QPixmap> ms_pixmap;
  static double ms_diameter;
};

#endif // BALL_H
