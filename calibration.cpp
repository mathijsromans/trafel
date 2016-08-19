#include "calibration.h"
#include "mouseping.h"
#include "pointerevent.h"
#include "utilities.h"
#include "transformscene.h"
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QSettings>
#include <QGraphicsPixmapItem>

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

Calibration::Calibration()
 : m_scene(0),
   m_status(Status::uninitialised),
   m_calibrationLights(),
   m_cornerPoints(),
   m_tableRect(QPoint(200, 200), QPoint(600, 600)),
   m_circle(0),
   m_tableRectItem(0),
   m_imageItem(0)
{
  QSettings settings("TafelSoft", "Tafel");
  settings.beginGroup("Calibration");
  unsigned int version = settings.value("version", 0u).toUInt();
  if ( version == ms_version )
  {
    m_status = static_cast<Status>(settings.value("status", static_cast<unsigned int>(Status::uninitialised)).toUInt());
    m_transform = settings.value("transform", QTransform()).value<QTransform>();
    m_tableRect = settings.value("tablerect", QRectF()).value<QRectF>();
    if ( m_status == Status::done )
    {
      m_status = Status::testing;
    }
  }
  settings.endGroup();
}

void Calibration::setScene(TransformScene* scene)
{
  m_scene = scene;
  if ( m_status != Status::done )
  {
    calibrate();
  }
  else
  {
    m_scene->doInit(m_tableRect);
  }
}

void Calibration::calibrate()
{
  if ( m_status == Status::testing )
  {
    m_testingLocation = 0.5 * ( m_tableRect.center() + m_tableRect.bottomRight());
    m_circle = m_scene->addEllipse( Utilities::squareAt(m_testingLocation,10), QPen(Qt::white,3));
    m_scene->showInfoText("Verify calibration");
    QPen pen(Qt::white);
    pen.setWidth(3);
    delete m_tableRectItem;
    m_tableRectItem = m_scene->addRect(m_tableRect, pen);
//    done(); // REMOVE ME TO VERIFY!!!
  }
  else
  {
    m_calibrationLights.clear();
    m_cornerPoints.clear();
    m_imageItem = new QGraphicsPixmapItem();
    m_imageItem->setPixmap(QPixmap::fromImage(QImage("grab_0.png")).scaledToWidth(500));
    m_imageItem->setPos(250, 250);
    m_imageItem->setZValue(-10);
    m_tableRectItem = m_scene->addRect(m_imageItem->boundingRect().translated(m_imageItem->pos()), QPen(Qt::white));
    m_scene->showInfoText("Verify camera position");
    m_scene->addItem(m_imageItem);
    m_status = Status::uninitialised;
  }
}

void Calibration::processTransformedMouseClick(PointerEvent e)
{
  if ( m_status == Status::transformDone )
  {
    newCornerPoint(e.getAny());
  }
  else if ( m_status == Status::testing )
  {
    if ( Utilities::dist( e.getAny(), m_testingLocation ) < 10 )
    {
      done();
    }
    else
    {
      clear();
      calibrate();
    }
  }
  else
  {
    m_scene->inputEvent(e);
  }
}

void Calibration::processEvent(PointerEvent e)
{
  if ( m_status != Status::done &&
       e.getAny().isNull() )
  {
    return; // ignore mouse release events when calibration is not done
  }
  if ( m_status == Status::uninitialised )
  {
    m_circle = m_scene->addEllipse(Utilities::squareAt(ms_calibrationCoordinates[m_calibrationLights.size()],20), QPen(Qt::green,3));
    m_scene->showInfoText("Click the points");
    delete m_imageItem;
    m_status = Status::cameraPositioned;
  }
  else if ( m_status == Status::cameraPositioned )
  {
    newCalibratePoint(e.getAny().toPoint());
  }
  else
  {
    e.transform(m_transform);
    for ( auto p : e.getPoints() )
    {
      if ( p.ping )
      {
        m_scene->addItem( new MousePing(p.point, PointerEvent::getQColor(p.color)) );
      }
    }
    processTransformedMouseClick(e);
  }

  if ( m_status == Status::transformDone )
  {
    std::string text = "Select corner " + std::to_string(m_cornerPoints.size()+1);
    m_scene->showInfoText(text);
  }
}

bool Calibration::isDone() const
{
  return m_status == Status::done;
}

void Calibration::clear()
{
  m_status = Status::uninitialised;
  m_calibrationLights.clear();
  m_cornerPoints.clear();
  delete m_circle; m_circle = 0;
  delete m_tableRectItem; m_tableRectItem = 0;
  m_tableRect = QRectF();
  m_transform.reset();
  m_testingLocation = QPointF();
  m_scene->showInfoText("");
}

void Calibration::done()
{
  m_status = Status::done;
  m_circle->hide();
  m_scene->showInfoText("");
  delete m_tableRectItem;
  m_scene->doInit(m_tableRect);
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
//    QPointF topLeft = m_cornerPoints[0];
//    QPointF bottomLeft = m_cornerPoints[1];
//    QPointF topRight = m_cornerPoints[2];
//    QPointF bottomRight = m_cornerPoints[3];
//    QPointF tableRectTopLeft(std::max(topLeft.x(), bottomLeft.x()), std::max(topLeft.y(), topRight.y()));
//    QPointF tableRectBottomRight(std::min(bottomRight.x(), topRight.x()), std::min(bottomLeft.y(), bottomRight.y()));
    QPointF top = 0.5 * ( m_cornerPoints[0] + m_cornerPoints[2] );
    QPointF bottom = 0.5 * ( m_cornerPoints[1] + m_cornerPoints[3] );
    QPointF left = 0.5 * ( m_cornerPoints[0] + m_cornerPoints[1] );
    QPointF right = 0.5 * ( m_cornerPoints[2] + m_cornerPoints[3] );
    QPointF tableRectTopLeft( left.x(), top.y() );
    QPointF tableRectBottomRight( right.x(), bottom.y() );
    m_tableRect = QRectF(tableRectTopLeft, tableRectBottomRight);
    qDebug() << "TABLE RECT IS " << m_tableRect;
    QPen pen(Qt::white);
    pen.setWidth(3);
    delete m_tableRectItem;
    m_tableRectItem = m_scene->addRect(m_tableRect, pen);
    done();
    QSettings settings("TafelSoft", "Tafel");
    settings.beginGroup("Calibration");
    settings.setValue("version", ms_version);
    settings.setValue("status", static_cast<unsigned int>(m_status));
    settings.setValue("transform", m_transform);
    settings.setValue("tablerect", m_tableRect);
    settings.endGroup();
  }
}
