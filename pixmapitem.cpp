#include "pixmapitem.h"

#include <QPainter>

PixmapItem::PixmapItem(QPixmap pixmap)
  : QGraphicsItem(),
    m_pixmap(pixmap)
{
}

QRectF PixmapItem::boundingRect() const
{
  QSize s = m_pixmap.size();
  return QRectF(-0.5*s.width(), -0.5*s.height(), s.width(), s.height());
}

void PixmapItem::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* /*option*/,
                      QWidget* /*widget*/)
{
  painter->drawPixmap(boundingRect().topLeft(), m_pixmap);
}
