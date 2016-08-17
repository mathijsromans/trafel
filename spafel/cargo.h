#ifndef CARGO_H
#define CARGO_H

#include "bodyitem.h"

class Body;
class Planet;
class Spaceship;

class Cargo : public BodyItem
{
public:
  enum class Location {
    origin,
    spaceship,
    destination,
  };

  enum class Action {
    dropoff,
    pickup,
    none
  };

public:
  Cargo(Planet* origin, Planet* destination);
  Cargo::Action notifyCollision(Spaceship* spaceship, Planet* planet);
  virtual void updateItem(const QRectF& tableRect, QPointF centreOfMass) override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
  virtual QRectF boundingRect() const override;
private:
  Body* getCurrentBody() const;
  QColor getLineColor() const;
private:
  Planet* m_origin;
  Planet* m_destination;
  Spaceship* m_spaceship;
  Location m_status;
};

#endif // CARGO_H
