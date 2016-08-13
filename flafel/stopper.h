#ifndef STOPPER_H
#define STOPPER_H

#include "nocopy.h"
#include <QPointF>
class QGraphicsLineItem;

class Stopper : NoCopy
{
public:
  explicit Stopper(QGraphicsLineItem* item);
  ~Stopper();
  void setEnd( QPointF newEnd );
  double length() const;
  QGraphicsLineItem* item;
};

#endif // STOPPER_H
