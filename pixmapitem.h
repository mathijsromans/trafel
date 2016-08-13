#ifndef CENTEREDPIXMAP_H
#define CENTEREDPIXMAP_H

#include <QGraphicsItem>
#include <QPixmap>

class PixmapItem : public QGraphicsItem
{
public:
  explicit PixmapItem(QPixmap pixmap);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter* painter,
                     const QStyleOptionGraphicsItem* option,
                     QWidget* widget);
private:
  QPixmap m_pixmap;
};

#endif // CENTEREDPIXMAP_H
