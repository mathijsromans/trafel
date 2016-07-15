#include "transformscene.h"
#include "utilities.h"
#include "mouseping.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QPainter>

#include <iostream>

TransformScene::TransformScene()
 : m_calibration(*this)
{
  setBackgroundBrush(Qt::black);
}

TransformScene::~TransformScene()
{
}

void TransformScene::calibrate()
{
  m_calibration.calibrate();
}

void TransformScene::mouseClick(QPointF /*p*/)
{
}

void TransformScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  m_calibration.processMouseClick(PointerEvent(std::array<QPointF, 3>{event->scenePos(), QPointF(0,0), QPointF(0,0)}));
}

void TransformScene::slotLightAt(PointerEvent e)
{
  m_calibration.processMouseClick( e );
}

const QRectF& TransformScene::getTableRect() const
{
  return m_calibration.getTableRect();
}
