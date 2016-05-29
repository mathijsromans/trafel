#include "transformscene.h"

#include <QDebug>
#include <QGraphicsEllipseItem>

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
 : m_calibrationMouseClicks(),
   m_circle(0)
{
  setBackgroundBrush(Qt::black);
  m_text = new QGraphicsTextItem();
  addItem(m_text);
  m_text->setPos(350,350);
  m_text->setHtml(QString("<div style='background-color: #ffff00;'>") + "(-,-)" + "</div>");
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

QRectF TransformScene::squareAt(QPointF p, double size)
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
  QString text = "("+QString::number(p.x())+","+QString::number(p.y())+")";
  m_text->setHtml(QString("<div style='background-color: #ffff00;'>") + text + "</div>");

  // calibrate
  if ( m_calibrationMouseClicks.size() >= ms_calibrationCoordinates.size() )
  {
    QPointF sceneP = 100.*m_transform.map(QPointF(p))+QPointF(300,300);
    qDebug() << p << " -> " << sceneP;
    m_circle->setRect(squareAt(sceneP,20));
    m_circle->show();
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
    QPolygonF poly;
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
      m_transform.map(QPointF(p));
      QPointF tp = 100.*m_transform.map(QPointF(p))+QPointF(300.,300.);
      qDebug() << p << " -> " << tp;
    }
    m_circle->hide();
  }

}

