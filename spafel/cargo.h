#ifndef CARGO_H
#define CARGO_H

#include "bodyitem.h"

class Body;
class Planet;
class Spaceship;

class Cargo : public BodyItem
{
public:
  enum class Status {
    origin,
    spaceship,
    destination,
  };
public:
  Cargo(Planet* origin, Planet* destination);
  void notifyCollision(Spaceship* spaceship, Planet* planet);
  virtual void updateItem(const QRectF& tableRect, QPointF centreOfMass) override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
  virtual QRectF boundingRect() const override;
private:
  Body* getCurrentBody() const;
private:
  Planet* m_origin;
  Planet* m_destination;
  Spaceship* m_spaceship;
  Status m_status;
};

#endif // CARGO_H
