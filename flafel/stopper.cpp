#include "stopper.h"

#include <QGraphicsLineItem>

Stopper::Stopper(QGraphicsLineItem* item)
  : item(item)
{
}

Stopper::~Stopper()
{
  delete item;
}

void Stopper::setEnd(QPointF newEnd)
{
  QLineF line = item->line();
  line.setP2(newEnd);
  item->setLine(line);
}

double Stopper::length() const
{
  return item->line().length();
}
