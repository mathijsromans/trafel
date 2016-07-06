#include "transformscene.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

const std::array<QPoint,4> TransformScene::ms_calibrationCoordinates = {QPoint{300,300},QPoint{400,300},QPoint{400,400},QPoint{300,400}};

namespace
{

template <typename T>
T sqr(T x) { return x*x; }

double dist(QPoint p1, QPoint p2)
{
  QPoint p = p1-p2;
  return sqrt(sqr(p.x()+sqr(p.y())));
}

}

TransformScene::TransformScene()
 : m_calibrationLights(),
   m_calibrated(false),
   m_circle(0)
{
  setBackgroundBrush(Qt::black);
  m_image = new QGraphicsPixmapItem();
  addItem(m_image);
}

TransformScene::~TransformScene()
{

}

void TransformScene::calibrate()
{
  m_calibrationLights.clear();
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

void TransformScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  mouseClick(event->pos());

}

void TransformScene::slotNewImage(QImage i)
{
  m_image->setPixmap(QPixmap::fromImage(i));
}

void TransformScene::slotLightAt(PointerEvent e)
{
  if ( m_calibrated )
  {
    e.transform(m_transform);
    m_circle->setRect(squareAt(e.getAny(),20));
    m_circle->show();
    qDebug() << "circle at " << m_circle->boundingRect();
    mouseClick(e.getAny());
  }
  else
  {
    newCalibratedPoint(e.getAny());
  }
}

void TransformScene::newCalibratedPoint(QPoint p)
{
  // calibrate
  for ( auto c : m_calibrationLights )
  {
    if ( dist(p,c) < 10 )
    {
      return;
    }
  }
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
    m_calibrated = true;
    init();
  }
}

