#ifndef PLANET_H
#define PLANET_H

#include "bodyitem.h"

class Planet : public BodyItem
{
public:
  Planet(Body* body);
  static unsigned int calcRadius(double mass);
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
  double m_radius;
};


#endif // PLANET_H
