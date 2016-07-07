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
  virtual void mouseClick(QPointF p);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  QRectF getTableRect() const;

private:

  virtual void init() = 0;

  void newCalibratedPoint(QPoint p);
  void newCornerPoint(QPointF pC);
  void processTransformedMouseClick(PointerEvent e);
  void processMouseClick(PointerEvent e);
  void showInfoText(const std::string& text) const;

private:
  static const std::array<QPoint,4> ms_calibrationCoordinates;
  std::vector<QPoint> m_calibrationLights;
  std::vector<QPointF> m_cornerPoints;
  QRectF m_tableRect;
  bool m_calibratedTransform;
  bool m_calibratedCorners;
  QGraphicsEllipseItem* m_circle;
  QGraphicsTextItem* m_infoText;
  QTransform m_transform;
};

#endif // TRANSFORMSCENE_H
