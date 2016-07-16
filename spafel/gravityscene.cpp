#include "gravityscene.h"

#include "spafel/bodyitem.h"
#include "spafel/environment.h"
#include "spaceship.h"

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
  const unsigned int stepsize = 120000.0;

  double calcOrbitalVelocity(double apogeeX, double eccentricity, double muCentralBody)
  {
    double apogeeVy = std::sqrt(muCentralBody/apogeeX*(1.0-eccentricity));
    return apogeeVy;
  }
}


GravityScene::GravityScene()
: TransformScene(),
  m_environment(new Environment()),
  m_bodyItems(),
  m_newBody(),
  m_trackItems(),
  m_tempBodyItem(0),
  m_sunItem(0),
  m_timer(new QTimer(this)),
  m_time(0)
{
}


GravityScene::~GravityScene()
{
}


BodyItem*
GravityScene::addBody(Body* body, const QColor& color)
{
  m_environment->addBody(body);
  m_bodies.push_back(body);
  BodyItem* bodyItem = new BodyItem(body, color);
  m_bodyItems.push_back(bodyItem);
  addItem(bodyItem);
  return bodyItem;
}

//void
//GravityScene::addSpaceship(Body* body)
//{
//  m_environment->addBody(body);
//  m_bodies.push_back(body);
//  Spaceship* ship = new Spaceship(body);
//  m_spaceships.push_back(ship);
//  addItem(ship);
//}

void
GravityScene::init()
{
  setBackgroundBrush(QBrush(Qt::black));

  createCelestialBodies();
  createSpaceShips();

  connect(m_timer, SIGNAL(timeout()), this, SLOT(step()));
  m_timer->start(1000/fps);
}


void
GravityScene::step()
{
  for ( unsigned int i = 0; i != 60*60*60 /stepsize; ++i )
  {
    m_environment->oneStep(stepsize, m_time);
    ++m_time;
  }
  for (BodyItem* planet : m_bodyItems)
  {
    planet->update(getTableRect(), m_time);
  }

  updateTrackItems();
}


void
GravityScene::mouseClick(QPointF /*point*/)
{
//  if (m_newBody)
//  {
//    Body* body = new Body(m_environment.get());
//    body->setMass(m_newBody->m_mass);
//    QPointF envPos = GravityScene::sceneToEnv(m_newBody->scenePos, getTableRect());
//    QPointF velVect = point - m_newBody->scenePos;
//    velVect = velVect * 2.0e2;
//    body->setPosition(envPos.x(), envPos.y());
//    body->setVelocity(velVect.x(), velVect.y());
//    addBody(body, Qt::green);
//    removeItem(m_tempBodyItem);
//    delete m_tempBodyItem;
//    m_tempBodyItem = 0;
//    m_newBody.release();
//  }
//  else
//  {
//    double mass = 5.0e28;
//    double radius = BodyItem::calcRadius(mass);
//    m_tempBodyItem = addEllipse(point.x()-radius, point.y()-radius, 2*radius, 2*radius, QPen(Qt::darkGreen), QBrush(Qt::darkGreen));
//    m_newBody.reset(new NewBodyData(mass, point));
//  }
}

void
GravityScene::removeBodyItem(BodyItem* bodyItem)
{
  m_bodyItems.erase( std::remove(m_bodyItems.begin(), m_bodyItems.end(), bodyItem), m_bodyItems.end() );
  removeItem(bodyItem);
  for (Body* body : m_environment->getBodies())
  {
    if (body == bodyItem->getBody())
    {
      m_environment->removeBody(body);
      for (const auto& lineItem : m_trackItems[body])
      {
        removeItem(lineItem);
        delete lineItem;
      }
      m_trackItems[body].clear();
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
GravityScene::updateTrackItems()
{
  for (const auto& body : m_environment->getBodies())
  {

    auto s1 = body->getState(m_time+50);
    auto s2 = body->getState(m_time+51);


    QLineF newLine(envToScene(QPointF(s2[0], s2[1]), getTableRect()), envToScene(QPointF(s1[0], s1[1]), getTableRect()));
    QGraphicsLineItem* line = addLine(newLine, QPen(Qt::red));
    line->setZValue(-10);
    m_trackItems[body].push_front(line);

    if (m_trackItems[body].size() > 50)
    {
      QGraphicsLineItem* line = m_trackItems[body].back();
      removeItem(line);
      m_trackItems[body].pop_back();
      delete line;
    }
  }

}


void
GravityScene::createCelestialBodies()
{
  const double sunMass = 1.989e30;
  Body* sun = new Body(0.0, 0.0, 0.0, 0.0, sunMass, m_environment.get());
  m_sunItem = addBody(sun, Qt::yellow);

  double earthEccentricity = 0.0167086;
  const double earthMass = 5.972e24;
  double earthX = 152.10e9;
  double earthVy = calcOrbitalVelocity(earthX, earthEccentricity, muSun);
  Body* earth = new Body(earthX, 0.0, 0.0, -earthVy, earthMass, m_environment.get());
  addBody(earth);

  double venusEccentricity = 0.006772;
  const double venusMass = 4.8675e24;
  double venusX = 108.939e9;
  double venusVy = calcOrbitalVelocity(venusX, venusEccentricity, muSun);
  Body* venus = new Body(venusX, 0.0, 0.0, -venusVy, venusMass, m_environment.get());
  addBody(venus);

  double mercuryEccentricity = 0.205630;
  const double mercuryMass = 3.3011e23;
  double mercuryX = 69.81690e9;
  double mercuryVy = calcOrbitalVelocity(mercuryX, mercuryEccentricity, muSun);
  Body* mercury = new Body(mercuryX, 0.0, 0.0, -mercuryVy, mercuryMass, m_environment.get());
  addBody(mercury);
}

void
GravityScene::createSpaceShips()
{
//  Body* ss1 = new Body(m_environment.get());
//  double earthEccentricity = 0.0167086;
//  double ss1X = 152.10e9;
//  double ss1Vy = calcOrbitalVelocity(ss1X, earthEccentricity, muSun);
//  ss1->setPosition(-ss1X, 0.0);
//  ss1->setVelocity(0.0, -ss1Vy);
//  addSpaceship(ss1);
}
