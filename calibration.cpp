#include "calibration.h"
#include "mouseping.h"
#include "pointerevent.h"
#include "utilities.h"
#include "transformscene.h"
#include <QDebug>
#include <QGraphicsEllipseItem>

const std::array<QPoint,4> Calibration::ms_calibrationCoordinates = {QPoint{200,200},QPoint{600,200},QPoint{600,600},QPoint{200,600}};


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

Calibration::Calibration(TransformScene& scene)
 : m_scene(scene),
   m_status(Status::uninitialised),
   m_calibrationLights(),
   m_cornerPoints(),
   m_tableRect(QPoint(200, 200), QPoint(600, 600)),
   m_circle(0),
   m_infoText(0),
   m_tableRectItem(0)
{
}

void Calibration::calibrate()
{
  m_calibrationLights.clear();
  m_cornerPoints.clear();
  m_status = Status::uninitialised;
  m_circle = m_scene.addEllipse(Utilities::squareAt(ms_calibrationCoordinates[m_calibrationLights.size()],20), QPen(Qt::green,3));
  m_infoText = m_scene.addText("Help Text");
  m_infoText->hide();
}

QRectF Calibration::getTableRect() const
{
  return m_tableRect;
}

void Calibration::processTransformedMouseClick(PointerEvent e)
{
  if ( m_status == Status::transformDone )
  {
    newCornerPoint(e.getAny());
    if ( m_status == Status::done )
    {
      m_infoText->hide();
      m_scene.init();
    }
  }
  else
  {
    m_scene.mouseClick(e.getAny());
  }
}

void Calibration::processMouseClick(PointerEvent e)
{
  if ( m_status == Status::uninitialised )
  {
    newCalibratePoint(e.getAny().toPoint());
  }
  else
  {
    e.transform(m_transform);
    for ( auto p : e.getPoints() )
    {
      m_scene.addItem( new MousePing(p.point, p.qcolor) );
    }
    processTransformedMouseClick(e);
  }

  if ( m_status == Status::transformDone )
  {
    std::string text = "Select corner " + std::to_string(m_cornerPoints.size()+1);
    showInfoText(text);
  }
}

void Calibration::newCalibratePoint(QPoint p)
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
    m_status = Status::transformDone;
  }
}

void Calibration::newCornerPoint(QPointF p)
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
    QPen pen(Qt::white);
    pen.setWidth(3);
    m_tableRectItem = m_scene.addRect(m_tableRect, pen);
    m_status = Status::done;
  }
}

void Calibration::showInfoText(const std::string& text) const
{
  QFont font;
  font.setPixelSize(40);
  m_infoText->setFont(font);
  m_infoText->setDefaultTextColor(Qt::white);
  m_infoText->setPlainText(QString::fromStdString(text));
  m_infoText->setPos(200, 200);
  m_infoText->show();
}

