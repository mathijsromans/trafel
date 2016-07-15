#ifndef BODYITEM_H
#define BODYITEM_H

#include <memory>

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QPointF>

class Body;
class QRectF;


class BodyItem : public QGraphicsEllipseItem
{
public:
  BodyItem(Body* body, const QColor& color=Qt::white);

  void update(const QRectF& tableRect);
  Body* getBody() const;
  unsigned int getRadius() const;
  static unsigned int calcRadius(double mass);

private:

private:
  Body* m_body;
  double m_radius;
};

#endif // BODYITEM_H
