#ifndef BODYITEM_H
#define BODYITEM_H

#include <QGraphicsItem>
class Body;

class BodyItem : public QGraphicsItem
{
public:
  BodyItem(Body* body);
  void update(const QRectF& tableRect, unsigned int time);
  Body* getBody() const;

private:
  Body* m_body;
};

#endif // BODYITEM_H
