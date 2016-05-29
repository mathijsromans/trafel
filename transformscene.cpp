#include "transformscene.h"

#include <QDebug>
#include <QGraphicsEllipseItem>

const std::array<QPoint,4> TransformScene::ms_calibrationCoordinates = {QPoint{300,300},QPoint{300,600},QPoint{600,600},QPoint{600,300}};

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
 : m_calibrationMouseClicks(),
   m_circle(0)
{

}

TransformScene::~TransformScene()
{

}

void TransformScene::calibrate()
{
  m_calibrationMouseClicks.clear();
  m_circle = addEllipse(squareAt(ms_calibrationCoordinates[m_calibrationMouseClicks.size()],20), QPen(Qt::green,3));
  qDebug() << "circle at " << m_circle->boundingRect();

}

QRectF TransformScene::squareAt(QPoint p, double size)
{
  return QRectF(p.x()-0.5*size, p.y()-0.5*size, size, size);
}

QRectF TransformScene::squareAt(double x, double y, double size)
{
  return QRectF(x-0.5*size, y-0.5*size, size, size);
}

void TransformScene::slotMouseClick(QPoint p)
{
  qDebug() << "MOUSE CLICK AT " << p;
  // calibrate
  if ( m_calibrationMouseClicks.size() >= ms_calibrationCoordinates.size() )
  {
    QPoint sceneP = 300*m_transform.map(p)+QPoint(300,300);
    m_circle->setRect(squareAt(sceneP,20));
    qDebug() << "circle at " << m_circle->boundingRect();
    return;
  }
  for ( auto c : m_calibrationMouseClicks )
  {
    if ( dist(p,c) < 10 )
    {
      return;
    }
  }
  m_calibrationMouseClicks.push_back(p);
  if ( m_calibrationMouseClicks.size() < ms_calibrationCoordinates.size() )
  {
    m_circle->setRect(squareAt(ms_calibrationCoordinates[m_calibrationMouseClicks.size()],20));
    qDebug() << "circle at " << m_circle->boundingRect();
  }
  else
  {
    QPolygon poly;
    for ( auto p : m_calibrationMouseClicks )
    {
      poly << p;
    }
    bool success = QTransform::quadToSquare(poly, m_transform);
    qDebug() << "CREATING TRANSFORM SUCCESS " << success;
    qDebug() << "TRANSFORM IS " << m_transform;

    for (unsigned int i = 0; i < m_calibrationMouseClicks.size(); ++i)
    {
      QPoint p = m_calibrationMouseClicks[i];
      qDebug() << p << " -> " << 300*m_transform.map(p)+QPoint(300,300);
    }
  }

}

