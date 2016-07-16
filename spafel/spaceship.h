#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <memory>

class Body;

class Spaceship : public QGraphicsPixmapItem
{
public:
  explicit Spaceship(Body* body);
  void update(const QRectF& tableRect);
protected:
  virtual void paint(QPainter* painter,
                     const QStyleOptionGraphicsItem* option,
                     QWidget* widget);
private:
  Body* m_body;
  static std::unique_ptr<QPixmap> ms_blueSpaceship;
};

#endif // SPACESHIP_H
