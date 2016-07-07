#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include "pointerevent.h"
#include <QGraphicsScene>
#include <QPoint>
#include <vector>

class QGraphicsPixmapItem;

class TransformScene : public QGraphicsScene
{
  Q_OBJECT

public:
  TransformScene();
  virtual ~TransformScene();
  void calibrate();

public slots:
  void slotLightAt( PointerEvent e );

protected:
  static QRectF squareAt(QPointF p, double size );
  static QRectF squareAt( double x, double y, double size );
  virtual void mouseClick(QPointF p);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:

  virtual void init() = 0;

  void newCalibratedPoint(QPoint p);
  void processMouseClick(PointerEvent e);

private:
  static const std::array<QPoint,4> ms_calibrationCoordinates;
  std::vector<QPoint> m_calibrationLights;
  bool m_calibrated;
  QGraphicsEllipseItem* m_circle;
  QTransform m_transform;
};

#endif // TRANSFORMSCENE_H
