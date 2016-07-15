#ifndef CLICKABLEITEM_H
#define CLICKABLEITEM_H

#include "pointerevent.h"

class ClickableItem
{
public:
  ClickableItem();
  virtual void mouseClick(const PointerEvent::CPoint& p) = 0;
};

#endif // CLICKABLEITEM_H
