#include "transformscene.h"
#include "utilities.h"
#include "mouseping.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QPainter>

#include <iostream>

const std::array<QPoint,4> TransformScene::ms_calibrationCoordinates = {QPoint{200,200},QPoint{600,200},QPoint{600,600},QPoint{200,600}};

namespace
{

bool compareX(QPointF A, QPointF B)
{
  return A.x() < B.x();
}

bool compareY(QPointF A, QPointF B)
{
  return A.y() < B.y();
}

}

TransformScene::TransformScene()
 : m_calibrationLights(),
   m_cornerPoints(),
   m_tableRect(QPoint(200, 200), QPoint(600, 600)),
   m_calibratedTransform(false),
   m_calibratedCorners(false),
   m_circle(0),
   m_infoText(0)
{
  setBackgroundBrush(Qt::black);
  m_infoText = addText("Help Text");
  m_infoText->hide();
}

TransformScene::~TransformScene()
{

}

void TransformScene::calibrate()
{
  m_calibrationLights.clear();
  m_cornerPoints.clear();
  m_calibratedTransform = false;
  m_calibratedCorners = false;
  m_circle = addEllipse(Utilities::squareAt(ms_calibrationCoordinates[m_calibrationLights.size()],20), QPen(Qt::green,3));
}

void TransformScene::mouseClick(QPointF /*p*/)
{
}

void TransformScene::processTransformedMouseClick(PointerEvent e)
{
  if ( !m_calibratedCorners )
  {
    newCornerPoint(e.getAny());
    if ( m_calibratedCorners )
    {
      m_infoText->hide();
      init();
    }
  }
  else
  {
    mouseClick(e.getAny());
  }
}

void TransformScene::processMouseClick(PointerEvent e)
{
  if ( !m_calibratedTransform )
  {
    newCalibratedPoint(e.getAny().toPoint());
  }
  else
  {
    e.transform(m_transform);
    addItem( new MousePing(e.getAny()) );
    processTransformedMouseClick(e);
  }

  if ( m_calibratedTransform && !m_calibratedCorners )
  {
    std::string text = "Select corner " + std::to_string(m_cornerPoints.size()+1);
    showInfoText(text);
  }
}

void TransformScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  processMouseClick(PointerEvent(std::array<QPointF, 3>{event->scenePos(), QPointF(0,0), QPointF(0,0)}));
}

void TransformScene::slotLightAt(PointerEvent e)
{
  processMouseClick( e );
}

void TransformScene::newCalibratedPoint(QPoint p)
{
  // calibrate
  m_calibrationLights.push_back(p);
  if ( m_calibrationLights.size() < ms_calibrationCoordinates.size() )
  {
    m_circle->setRect(Utilities::squareAt(ms_calibrationCoordinates[m_calibrationLights.size()],20));
  }
  else
  {
    QPolygonF coords;
    for ( QPoint p : ms_calibrationCoordinates )
    {
      coords << p;
    }
    QPolygonF lights;
    for ( QPoint p : m_calibrationLights )
    {
      lights << p;
    }
    bool success = QTransform::quadToQuad(lights, coords, m_transform);
    qDebug() << "CREATING TRANSFORM SUCCESS " << success;

    m_circle->hide();
    m_calibratedTransform = true;
  }
}

void TransformScene::newCornerPoint(QPointF p)
{
  if ( m_cornerPoints.size() < 4 )
  {
    m_cornerPoints.push_back(p);
  }

  if ( m_cornerPoints.size() == 4 )
  {
    std::sort(m_cornerPoints.begin(), m_cornerPoints.end(), compareX);
    std::sort(m_cornerPoints.begin(), std::next(m_cornerPoints.begin(), 2), compareY);
    std::sort(std::next(m_cornerPoints.begin(), 2), m_cornerPoints.end(), compareY);
    QPointF topLeft = m_cornerPoints[0];
    QPointF bottomLeft = m_cornerPoints[1];
    QPointF topRight = m_cornerPoints[2];
    QPointF bottomRight = m_cornerPoints[3];
    QPointF tableRectTopLeft(std::max(topLeft.x(), bottomLeft.x()), std::max(topLeft.y(), topRight.y()));
    QPointF tableRectBottomRight(std::min(bottomRight.x(), topRight.x()), std::min(bottomLeft.y(), bottomRight.y()));
    m_tableRect = QRectF(tableRectTopLeft, tableRectBottomRight);
    qDebug() << "TABLE RECT IS " << m_tableRect;
    m_calibratedCorners = true;
  }
}

QRectF TransformScene::getTableRect() const
{
  return m_tableRect;
}

void TransformScene::showInfoText(const std::string& text) const
{
  QFont font;
  font.setPixelSize(40);
  m_infoText->setFont(font);
  m_infoText->setDefaultTextColor(Qt::white);
  m_infoText->setPlainText(QString::fromStdString(text));
  m_infoText->setPos(200, 200);
  m_infoText->show();
}
