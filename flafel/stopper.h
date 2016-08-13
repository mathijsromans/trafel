#ifndef STOPPER_H
#define STOPPER_H

#include "nocopy.h"

class QGraphicsLineItem;

class Stopper : NoCopy
{
public:
  Stopper(QGraphicsLineItem* item);
  QGraphicsLineItem* item;
};

#endif // STOPPER_H
