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
private:
  void initPixmaps();
private:
  QPixmap m_pixmap;
  static std::unique_ptr<QPixmap> ms_redSpaceship;
  static std::unique_ptr<QPixmap> ms_greenSpaceship;
  static std::unique_ptr<QPixmap> ms_blueSpaceship;
};

#endif // SPACESHIP_H
