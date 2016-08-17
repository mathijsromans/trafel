#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "bodyitem.h"
#include <QPixmap>
#include <memory>
class Body;

class Spaceship : public BodyItem
{
public:
  explicit Spaceship(Body* body, unsigned int id);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter* painter,
                     const QStyleOptionGraphicsItem* option,
                     QWidget* widget);
  unsigned int getPlayerId() const;
  void setShowRocketOn();
  void setHasCargo(bool hasCargo);
  bool hasCargo() const;
private:
  void initPixmaps();
private:
  bool m_hasCargo;
  QPixmap m_pixmap;
  QPixmap m_pixmapRocketOff;
  unsigned int m_playerId;
  int m_rocketOn;
  static std::unique_ptr<QPixmap> ms_redSpaceship;
  static std::unique_ptr<QPixmap> ms_greenSpaceship;
  static std::unique_ptr<QPixmap> ms_blueSpaceship;
  static std::unique_ptr<QPixmap> ms_redSpaceshipRocketOff;
  static std::unique_ptr<QPixmap> ms_greenSpaceshipRocketOff;
  static std::unique_ptr<QPixmap> ms_blueSpaceshipRocketOff;
};

#endif // SPACESHIP_H
