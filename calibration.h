#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <array>
#include <vector>
#include <QPoint>
#include <QPointF>
#include <QRectF>
#include <QTransform>
class TransformScene;
class QGraphicsRectItem;
class QGraphicsEllipseItem;
class PointerEvent;

class Calibration
{
public:
  explicit Calibration();
  void setScene( TransformScene* scene );
  void processEvent(PointerEvent e);

private:
  void calibrate();
  void clear();
  void done();
  void newCalibratePoint(QPoint p);
  void newCornerPoint(QPointF pC);
  void processTransformedMouseClick(PointerEvent e);

private:
  enum class Status { uninitialised, transformDone, testing, done };
  TransformScene* m_scene;
  Status m_status;
  std::vector<QPoint> m_calibrationLights;
  std::vector<QPointF> m_cornerPoints;
  QRectF m_tableRect;
  QGraphicsEllipseItem* m_circle;  
  QGraphicsRectItem* m_tableRectItem;
  QTransform m_transform;
  QPointF m_testingLocation;
  static const unsigned int ms_version = 1;
  static const std::array<QPoint,4> ms_calibrationCoordinates;
};

#endif // CALIBRATION_H
