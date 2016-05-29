#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include <QGraphicsScene>
#include <QPoint>
#include <vector>

class TransformScene : public QGraphicsScene
{
  Q_OBJECT

public:
  TransformScene();
  virtual ~TransformScene();
  void calibrate();

public slots:
  void slotMouseClick( QPoint p );

protected:
  static QRectF squareAt( QPoint p, double size );
  static QRectF squareAt( double x, double y, double size );

private:
  static const std::array<QPoint,3> ms_calibrationCoordinates;
  std::vector<QPoint> m_calibrationMouseClicks;
  QGraphicsEllipseItem* m_circle;
};

#endif // TRANSFORMSCENE_H
