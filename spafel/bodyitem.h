#ifndef BODYITEM_H
#define BODYITEM_H

#include <QGraphicsItem>
class Body;

class BodyItem : public QGraphicsItem
{
public:
  BodyItem(Body* body);
  void updateItem(const QRectF& tableRect, QPointF centreOfMass);
  Body* getBody() const;

private:
  Body* m_body;
};

#endif // BODYITEM_H
