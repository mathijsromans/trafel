#ifndef RAFELITEM_H
#define RAFELITEM_H

#include <QGraphicsItem>

class RafelItem : public QGraphicsItem
{
public:
  enum class Type { Red, Green, Blue, MAX };
  explicit RafelItem(double radius, Type type);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
  Type getType() const;

private:
  double m_radius;
  Type m_type;
};

#endif // RAFELITEM_H
