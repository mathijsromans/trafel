#ifndef PLANET_H
#define PLANET_H

#include "bodyitem.h"

#include <QColor>

class Planet : public BodyItem
{
public:
  Planet(Body* body, const QColor& color);
  static unsigned int calcRadius(double mass);
  virtual QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
  QColor getColor() const;

private:
  double m_radius;
  QColor m_color;
};


#endif // PLANET_H
