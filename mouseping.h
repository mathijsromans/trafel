#ifndef MOUSEPING_H
#define MOUSEPING_H

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QPropertyAnimation>

class MousePing : public QObject, public QGraphicsEllipseItem
{
  Q_OBJECT
  Q_PROPERTY(double size READ getSize WRITE setSize)
public:
  explicit MousePing(QPointF p, const QColor& color);
  ~MousePing();
  double getSize() const;
  void setSize(double size);
private slots:
  void animationFinished();
private:
  double m_size;
  QPropertyAnimation m_animation;
};

#endif // MOUSEPING_H
