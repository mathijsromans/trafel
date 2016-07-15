#ifndef BODYITEM_H
#define BODYITEM_H

#include <memory>

#include <QPointF>
#include <QColor>

class Body;
class QGraphicsEllipseItem;
class QRectF;

class BodyItem
{
public:
  BodyItem(Body* body, const QColor& color=Qt::white);

  void update(const QRectF& tableRect);
  QGraphicsEllipseItem* getItem() const;
  Body* getBody() const;
  unsigned int getRadius() const;
  static unsigned int calcRadius(double mass);

private:

private:
  std::unique_ptr<Body> m_body;
  QGraphicsEllipseItem* m_item;
  double m_radius;
};

#endif // BODYITEM_H
