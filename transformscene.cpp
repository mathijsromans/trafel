#include "transformscene.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QPainter>

#include <iostream>

const std::array<QPoint,4> TransformScene::ms_calibrationCoordinates = {QPoint{200,200},QPoint{600,200},QPoint{600,600},QPoint{200,600}};

namespace
{

bool compareX(QPoint A, QPoint B)
{
  return A.x() < B.x();
}

bool compareY(QPoint A, QPoint B)
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
  m_circle = addEllipse(squareAt(ms_calibrationCoordinates[m_calibrationLights.size()],20), QPen(Qt::green,3));
  qDebug() << "circle at " << m_circle->boundingRect();

}

QRectF TransformScene::squareAt(QPointF p, double size)
{
  return QRectF(p.x()-0.5*size, p.y()-0.5*size, size, size);
}

QRectF TransformScene::squareAt(double x, double y, double size)
{
  return QRectF(x-0.5*size, y-0.5*size, size, size);
}

void TransformScene::mouseClick(QPointF /*p*/)
{
}

void TransformScene::processMouseClick(PointerEvent e)
{
  if ( m_calibratedTransform && m_calibratedCorners )
  {
    e.transform(m_transform);
    m_circle->setRect(squareAt(e.getAny(),20));
    m_circle->show();
    qDebug() << "circle at " << m_circle->boundingRect();
    mouseClick(e.getAny());
  }
  else
  {
    if ( !m_calibratedTransform )
    {
      newCalibratedPoint(e.getAny());
    }
    else if ( !m_calibratedCorners )
    {
      newCornerPoint(e.getAny());
    }

    if ( m_calibratedTransform && !m_calibratedCorners )
    {
      std::string text = "Select corner " + std::to_string(m_cornerPoints.size()+1);
      showInfoText(text);
    }

    if ( m_calibratedTransform && m_calibratedCorners )
    {
      m_infoText->hide();
      init();
    }
  }
}

void TransformScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  processMouseClick(PointerEvent(std::array<QPoint, 3>{event->scenePos().toPoint(), QPoint(0,0), QPoint(0,0)}));
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
    m_circle->setRect(squareAt(ms_calibrationCoordinates[m_calibrationLights.size()],20));
    qDebug() << "circle at " << m_circle->boundingRect();
  }
  else
  {
    QPolygonF coords;
    for ( QPoint p : ms_calibrationCoordinates )
    {
      coords << p;
    }
    QPolygonF ligths;
    for ( QPoint p : m_calibrationLights )
    {
      ligths << p;
    }
    bool success = QTransform::quadToQuad(ligths, coords, m_transform);
    qDebug() << "CREATING TRANSFORM SUCCESS " << success;
    qDebug() << "TRANSFORM IS " << m_transform;

    m_circle->hide();
    m_calibratedTransform = true;
  }
}

void TransformScene::newCornerPoint(QPoint p)
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
    QPoint topLeft = m_cornerPoints[0];
    QPoint bottomLeft = m_cornerPoints[1];
    QPoint topRight = m_cornerPoints[2];
    QPoint bottomRight = m_cornerPoints[3];
    QPoint tableRectTopLeft(std::max(topLeft.x(), bottomLeft.x()), std::max(topLeft.y(), topRight.y()));
    QPoint tableRectBottomRight(std::min(bottomRight.x(), topRight.x()), std::min(bottomLeft.y(), bottomRight.y()));
    m_tableRect = QRect(tableRectTopLeft, tableRectBottomRight);
    qDebug() << "TABLE RECT IS " << m_tableRect;
    m_calibratedCorners = true;
  }
}

QRect TransformScene::getTableRect() const
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
