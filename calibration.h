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
class QGraphicsTextItem;
class PointerEvent;

class Calibration
{
public:
  explicit Calibration( TransformScene& scene );
  void calibrate();
  QRectF getTableRect() const;
  void processMouseClick(PointerEvent e);

private:
  void clear();
  void newCalibratePoint(QPoint p);
  void newCornerPoint(QPointF pC);
  void processTransformedMouseClick(PointerEvent e);
  void showInfoText(const std::string& text) const;

private:
  enum class Status { uninitialised, transformDone, testing, done };
  TransformScene& m_scene;
  Status m_status;
  std::vector<QPoint> m_calibrationLights;
  std::vector<QPointF> m_cornerPoints;
  QRectF m_tableRect;
  QGraphicsEllipseItem* m_circle;
  QGraphicsTextItem* m_infoText;
  QGraphicsRectItem* m_tableRectItem;
  QTransform m_transform;
  QPointF m_testingLocation;
  static const unsigned int ms_version = 1;
  static const std::array<QPoint,4> ms_calibrationCoordinates;
};

#endif // CALIBRATION_H
