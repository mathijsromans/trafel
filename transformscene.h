#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include <QGraphicsScene>
#include <QPoint>
#include <vector>

class QGraphicsTextItem;
class QGraphicsPixmapItem;

class TransformScene : public QGraphicsScene
{
  Q_OBJECT

public:
  TransformScene();
  virtual ~TransformScene();
  void calibrate();

public slots:
  void slotLightAt( QPoint p, QImage i );

protected:
  static QRectF squareAt(QPointF p, double size );
  static QRectF squareAt( double x, double y, double size );
  virtual void mouseClick(QPointF p);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:

  void newCalibratedPoint(QPoint p);

private:
  static const std::array<QPoint,4> ms_calibrationCoordinates;
  std::vector<QPoint> m_calibrationMouseClicks;
  bool m_calibrated;
  QGraphicsEllipseItem* m_circle;
  QTransform m_transform;
  QGraphicsTextItem* m_text;
  QGraphicsPixmapItem* m_image;
};

#endif // TRANSFORMSCENE_H
