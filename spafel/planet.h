#ifndef PLANET_H
#define PLANET_H

#include "bodyitem.h"
#include <QColor>

class Planet : public BodyItem
{
public:
  Planet(Body* body, const QColor& color=Qt::white);
  static unsigned int calcRadius(double mass);
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
  double m_radius;
  QColor m_color;
};


#endif // PLANET_H
