#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include "calibration.h"
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
  virtual void mouseClick(QPointF p);
  virtual void init() = 0;

public slots:
  void slotLightAt( PointerEvent e );

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  const QRectF& getTableRect() const;

private:
  Calibration m_calibration;
};

#endif // TRANSFORMSCENE_H
