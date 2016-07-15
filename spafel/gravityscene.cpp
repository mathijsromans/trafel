#include "gravityscene.h"

#include "bodyitem.h"

#include "environment.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

#include <cmath>

namespace
{
  const double AU = 149.6e9;
  const double muSun = 1.32712440018e20;
  const double zoom = 0.4;
  const unsigned int fps = 60;
  const unsigned int stepsize = 120.0;

  double calcOrbitalVelocity(double apogeeX, double eccentricity, double muCentralBody)
  {
    double apogeeVy = std::sqrt(muCentralBody/apogeeX*(1.0-eccentricity));
    return apogeeVy;
  }
}


GravityScene::GravityScene()
: TransformScene(),
  m_environment(new Environment()),
  m_newBody(),
  m_tempBodyItem(0),
  m_sunItem(0),
  m_timer(new QTimer(this))
{
}


GravityScene::~GravityScene()
{
}


BodyItem*
GravityScene::addBody(Body* body, const QColor& color)
{
  m_environment->addBody(body);
  BodyItem* bodyItem = new BodyItem(body, color);
  m_bodyItems.push_back(bodyItem);
  m_bodies.push_back(body);
  addItem(bodyItem);
  return bodyItem;
}


void
GravityScene::init()
{
  setBackgroundBrush(QBrush(Qt::black));

  createCelestialBodies();

  connect(m_timer, SIGNAL(timeout()), this, SLOT(step()));
  m_timer->start(1000/fps);
}


void
GravityScene::step()
{
  double tEnd = 60*60*6.0;
  m_environment->oneStep(tEnd, stepsize);

  for (BodyItem* planet : m_bodyItems)
  {
    planet->update(getTableRect());
  }

  detectCollisionWithSun();
}


void
GravityScene::mouseClick(QPointF point)
{
  if (m_newBody)
  {
    Body* body = new Body(m_environment.get());
    body->setMass(m_newBody->m_mass);
    QPointF envPos = GravityScene::sceneToEnv(m_newBody->scenePos, getTableRect());
    QPointF velVect = point - m_newBody->scenePos;
    velVect = velVect * 2.0e2;
    body->setPosition(envPos.x(), envPos.y());
    body->setVelocity(velVect.x(), velVect.y());
    addBody(body, Qt::green);
    removeItem(m_tempBodyItem);
    delete m_tempBodyItem;
    m_tempBodyItem = 0;
    m_newBody.release();
  }
  else
  {
    double mass = 5.0e28;
    double radius = BodyItem::calcRadius(mass);
    m_tempBodyItem = addEllipse(point.x()-radius, point.y()-radius, 2*radius, 2*radius, QPen(Qt::darkGreen), QBrush(Qt::darkGreen));
    m_newBody.reset(new NewBodyData(mass, point));
  }
}


void
GravityScene::detectCollisionWithSun()
{
  for (const auto& bodyItem : m_bodyItems)
  {
    if (bodyItem != m_sunItem && bodyItem->collidesWithItem(m_sunItem))
    {
      std::cout << "COLLISION" << std::endl;
      // conserve momentum
      Body* sun = m_sunItem->getBody();
      const Body* other = bodyItem->getBody();
      double combinedMass = sun->getMass() + other->getMass();
      double momentumStartX = other->getState()[2] * other->getMass() + sun->getState()[2] * sun->getMass();
      double momentumStartY = other->getState()[3] * other->getMass() + sun->getState()[3] * sun->getMass();
      double vxSunNew = momentumStartX / combinedMass;
      double vySunNew = momentumStartY / combinedMass;
      sun->setVelocity(vxSunNew, vySunNew);
      // remove body and item from the scene
      removeBodyItem(bodyItem);
    }
  }
}


void
GravityScene::removeBodyItem(BodyItem* bodyItem)
{
  m_bodyItems.erase( std::remove(m_bodyItems.begin(), m_bodyItems.end(), bodyItem), m_bodyItems.end() );
  removeItem(bodyItem);
  for (Body* body : m_bodies)
  {
    if (body == bodyItem->getBody())
    {
      m_environment->removeBody(body);
      delete body;
      break;
    }
  }
  delete bodyItem;
}


QPointF
GravityScene::envToScene(const QPointF& point, const QRectF& tableRect)
{
  double sceneCentreOffsetX = tableRect.topLeft().x() + tableRect.width() / 2.0;
  double sceneCentreOffsetY = tableRect.topLeft().y() + tableRect.height() / 2.0;
  double scaleFactor = getScaleFactor(tableRect);
  double x = point.x()/AU * scaleFactor + sceneCentreOffsetX;
  double y = point.y()/AU * scaleFactor + sceneCentreOffsetY;
  QPointF pointScene(x, y);
  return pointScene;
}


QPointF
GravityScene::sceneToEnv(const QPointF& point, const QRectF& tableRect)
{
  double sceneCentreOffsetX = tableRect.topLeft().x() + tableRect.width() / 2.0;
  double sceneCentreOffsetY = tableRect.topLeft().y() + tableRect.height() / 2.0;
  double scaleFactor = getScaleFactor(tableRect);
  double x = (point.x() - sceneCentreOffsetX) * AU / scaleFactor;
  double y = (point.y() - sceneCentreOffsetY) * AU / scaleFactor;
  return QPointF(x, y);
}


double
GravityScene::getScaleFactor(const QRectF& tableRect)
{
  return std::min(tableRect.width(), tableRect.height()) * zoom;
}


void
GravityScene::createCelestialBodies()
{
  const double sunMass = 1.989e30;
  Body* sun = new Body(m_environment.get());
  sun->setMass(sunMass);
  sun->setVelocity(0.0, 0.0);
  sun->setPosition(0.0, 0.0);
  m_sunItem = addBody(sun, Qt::yellow);

  Body* earth = new Body(m_environment.get());
  double earthEccentricity = 0.0167086;
  const double earthMass = 5.972e24;
  earth->setMass(earthMass);
  double earthX = 152.10e9;
  double earthVy = calcOrbitalVelocity(earthX, earthEccentricity, muSun);
  earth->setPosition(earthX, 0.0);
  earth->setVelocity(0.0, -earthVy);
  addBody(earth);

  Body* venus = new Body(m_environment.get());
  double venusEccentricity = 0.006772;
  const double venusMass = 4.8675e24;
  venus->setMass(venusMass);
  double venusX = 108.939e9;
  double venusVy = calcOrbitalVelocity(venusX, venusEccentricity, muSun);
  venus->setPosition(venusX, 0.0);
  venus->setVelocity(0.0, -venusVy);
  addBody(venus);

  Body* mercury = new Body(m_environment.get());
  double mercuryEccentricity = 0.205630;
  const double mercuryMass = 3.3011e23;
  mercury->setMass(mercuryMass);
  double mercuryX = 69.81690e9;
  double mercuryVy = calcOrbitalVelocity(mercuryX, mercuryEccentricity, muSun);
  mercury->setPosition(mercuryX, 0.0);
  mercury->setVelocity(0.0, -mercuryVy);
  addBody(mercury);
}
