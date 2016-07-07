#ifndef MOUSEPING_H
#define MOUSEPING_H

#include <QGraphicsEllipseItem>

class MousePing : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT
  Q_PROPERTY(double size READ getSize WRITE setSize)
public:
  MousePing(QPointF p, double size);
  double getSize() const;
  void setSize(double size);
private:
  double m_size;
};

#endif // MOUSEPING_H
