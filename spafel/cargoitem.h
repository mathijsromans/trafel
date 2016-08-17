#ifndef CARGOITEM_H
#define CARGOITEM_H

#include "bodyitem.h"

class Body;
class Planet;
class Spaceship;

class CargoItem : public BodyItem
{
public:
  enum class Status {
    origin,
    spaceship,
    destination,
  };
public:
  CargoItem(Planet* origin, Planet* destination);
  void setSpaceship(Spaceship* spaceship);
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

#endif // CARGOITEM_H
