#ifndef CLICKABLEITEM_H
#define CLICKABLEITEM_H

#include "pointerevent.h"

class ClickableItem
{
public:
  ClickableItem();
  virtual void eventClick(const PointerEvent::CPoint& p) = 0;
};

#endif // CLICKABLEITEM_H
