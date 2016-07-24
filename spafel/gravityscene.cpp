#include "gravityscene.h"

#include "button.h"
#include "spafel/planet.h"
#include "spafel/environment.h"
#include "spaceship.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QSignalMapper>
#include <QTimer>

#include <cmath>

namespace
{
  const double AU = 149.6e9;
  const double sunMass = 1e27;
  const double zoom = 0.4;
  const double fps = 30;            // number of frames per second
  const double speedup = 100000000;  // speedup factor, number of seconds per second
  const double stepsize = speedup/fps;
}


GravityScene::GravityScene()
: TransformScene(),
  m_environment(new Environment(stepsize)),
  m_bodyItems(),
  m_newBody(),
  m_trackItems(),
  m_tempPlanet(0),
  m_timer(new QTimer(this))
{
}


GravityScene::~GravityScene()
{
}


void GravityScene::addBody(Body* body)
{
  m_environment->addBody(body);
  Planet* bodyItem = new Planet(body);
  m_bodyItems.push_back(bodyItem);
  addItem(bodyItem);
}

void
GravityScene::addSpaceship(Body* body, unsigned int id)
{
  m_environment->addSpaceship(body);
  Spaceship* ship = new Spaceship(body, id);
  m_bodyItems.push_back(ship);
  addItem(ship);
}

void
GravityScene::init()
{
  setBackgroundBrush(QBrush(Qt::black));

  createCelestialBodies();
  createSpaceShips();
  m_environment->init();

  connect(m_timer, SIGNAL(timeout()), this, SLOT(step()));
  m_timer->start(1000/fps);

//  QSignalMapper* signalMapper = new QSignalMapper(this);
//  std::

//  connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
//        signalMapper->setMapping(button, texts[i]);
//        gridLayout->addWidget(button, i / 3, i % 3);
//    }

//    connect(signalMapper, SIGNAL(mapped(QString)),
//            this, SIGNAL(clicked(QString)));




  Button* button = new Button("Up");
  button->setPos(getTableRect().bottomLeft());
  addItem( button );
  connect( button, SIGNAL(pressed()), this, SLOT(slotButtonPressed()) );
}


void
GravityScene::step()
{
  m_environment->oneStep();
  for (BodyItem* bodyItem : m_bodyItems)
  {
    bodyItem->updateItem(getTableRect());
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
//    removeItem(m_tempPlanet);
//    delete m_tempPlanet;
//    m_tempPlanet = 0;
//    m_newBody.release();
//  }
//  else
//  {
//    double mass = 5.0e28;
//    double radius = Planet::calcRadius(mass);
//    m_tempPlanet = addEllipse(point.x()-radius, point.y()-radius, 2*radius, 2*radius, QPen(Qt::darkGreen), QBrush(Qt::darkGreen));
//    m_newBody.reset(new NewBodyData(mass, point));
//  }
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

void GravityScene::slotButtonPressed()
{
  m_environment->boost(0, Body::Direction::up);

}

void
GravityScene::updateTrackItems()
{
  for (Body* body : m_environment->getBodies())
  {
    std::deque<QGraphicsLineItem*>& lines = m_trackItems[body];
    unsigned int createFromTime = -1;
    if ( body->trackChanged() )
    {
      for ( QGraphicsLineItem* line : lines )
      {
        removeItem(line);
        delete line;
      }
      lines.clear();
      createFromTime = 0;
    }
    else
    {
      QGraphicsLineItem* line = lines.front();
      lines.pop_front();
      removeItem(line);
      delete line;
      createFromTime = Body::timeAhead - 2;
    }
    for ( unsigned int t = createFromTime; t != Body::timeAhead - 1; ++t )
    {
      unsigned int time = t + m_environment->getCurrentTime();
      auto s1 = body->getState(time);
      auto s2 = body->getState(time + 1);
      QLineF newLine(envToScene(QPointF(s2[0], s2[1]), getTableRect()), envToScene(QPointF(s1[0], s1[1]), getTableRect()));
      QGraphicsLineItem* line = addLine(newLine, QPen(body->getColor()));
      line->setZValue(-10);
      lines.push_back(line);
    }
  }
}

void
GravityScene::createCelestialBodies()
{
  Body* sun = new Body(0.0, 0.0, 0.0, 0.0, sunMass, Qt::yellow, m_environment.get());
  addBody(sun);

  double earthEccentricity = 0.0167086;
  const double earthMass = 5.972e24;
  double earthX = 152.10e9;
  double earthVy = Environment::calcOrbitalVelocity(earthX, earthEccentricity, sunMass);
  Body* earth = new Body(earthX, 0.0, 0.0, -earthVy, earthMass, Qt::white, m_environment.get());
  addBody(earth);

  double venusEccentricity = 0.006772;
  const double venusMass = 4.8675e24;
  double venusX = 108.939e9;
  double venusVy = Environment::calcOrbitalVelocity(venusX, venusEccentricity, sunMass);
  Body* venus = new Body(venusX, 0.0, 0.0, -venusVy, venusMass, Qt::white, m_environment.get());
  addBody(venus);

  double mercuryEccentricity = 0.205630;
  const double mercuryMass = 3.3011e23;
  double mercuryX = 69.81690e9;
  double mercuryVy = Environment::calcOrbitalVelocity(mercuryX, mercuryEccentricity, sunMass);
  Body* mercury = new Body(mercuryX, 0.0, 0.0, -mercuryVy, mercuryMass, Qt::white, m_environment.get());
  addBody(mercury);
}

void
GravityScene::createSpaceShips()
{
  const double s1X = 100e9;
  const double s1Vy = Environment::calcOrbitalVelocity(s1X, 0, sunMass);
  std::array<QColor, ms_numSpaceships> colors{QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue)};
  for ( unsigned int i = 0; i != ms_numSpaceships; ++i )
  {
    double angle = 2 * M_PI * i / ms_numSpaceships;
    double x = sin(angle) * s1X;
    double y = cos(angle) * s1X;
    double vx = cos(angle) * s1Vy;
    double vy = -sin(angle) * s1Vy;
    Body* s1 = new Body(x, y, vx, vy, 0, colors[i], m_environment.get());
    addSpaceship(s1, i);
  }
}
