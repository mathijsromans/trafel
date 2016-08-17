#include "gravityscene.h"

#include "button.h"
#include "spafel/planet.h"
#include "spafel/environment.h"
#include "spafel/spaceship.h"
#include "spafel/cargo.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QSignalMapper>

#include <cmath>

namespace
{
  const double AU = 149.6e9;
  const double sunMass = 1e27;
  const double zoom = 0.3;
  const double fps = 30;            // number of frames per second
  const double speedup = 50000000;  // speedup factor, number of seconds per second
  const double stepsize = speedup/fps;
  const unsigned int winningScore = 50;
}

const std::array<std::string, GravityScene::ms_numControl> GravityScene::ms_buttonTexts = {"U", "D", "L", "R"};

GravityScene::GravityScene() :
  TransformScene(),
  m_environment(new Environment(stepsize)),
  m_bodyItems(),
  m_planets(),
  m_spaceships(),
  m_cargos(),
  m_trackItems(),
  m_tempPlanet(0),
  m_scores()
{
  static std::random_device rd;
  m_randomGenerator.seed(rd());
}

GravityScene::~GravityScene()
{
}

void
GravityScene::addPlanet(Body* body, const QColor& lineColor)
{
  m_environment->addBody(body);
  Planet* bodyItem = new Planet(body, lineColor);
  m_planets.push_back(bodyItem);
  m_bodyItems.push_back(bodyItem);
  addItem(bodyItem);
}

void
GravityScene::addSpaceship(Body* body, unsigned int id)
{
  m_environment->addSpaceship(body);
  Spaceship* ship = new Spaceship(body, id);
  m_spaceships[id] = ship;
  m_bodyItems.push_back(ship);
  m_scores[id] = 0;
  addItem(ship);
}

void
GravityScene::init()
{
  setBackgroundBrush(QBrush(Qt::black));

  createCelestialBodies();
  createSpaceShips();
  createCargo();
  m_environment->init();

  std::array<ButtonInLayout, 4> buttons;
  buttons[0] = { Control::up, QPoint(0, 1) };
  buttons[1] = { Control::left, QPoint(-1, 0) };
  buttons[2] = { Control::down, QPoint(0, 0) };
  buttons[3] = { Control::right, QPoint(1, 0) };

  QSignalMapper* signalMapper = new QSignalMapper(this);
  for ( unsigned int i = 0; i != getNumPlayers(); ++i )
  {
    QLineF playerLine = getPlayerPosition(i);
    QPointF origin = playerLine.p1();
    QPointF xDir = playerLine.p1() - playerLine.normalVector().p2();
    QPointF yDir = playerLine.p2() - playerLine.p1();
    for ( unsigned int j = 0; j != buttons.size(); ++j )
    {
      unsigned int control = static_cast<unsigned int>(buttons[j].control);
      Button* button = new Button(ms_buttonTexts[control]);
      QRectF buttonRect = button->boundingRect();
      QPointF buttonSize = buttonRect.bottomRight() - buttonRect.topLeft();
      button->setTransformOriginPoint(0.5 * buttonSize);
      button->setPos(origin + 1.25 * buttonSize.x() * buttons[j].pos.x() * xDir
                            + 1.25 * buttonSize.y() * buttons[j].pos.y() * yDir
                            - 0.5*buttonSize );
      button->setRotation(90-playerLine.angle());
      addItem( button );
      connect(button, SIGNAL(pressed()), signalMapper, SLOT(map()));
      signalMapper->setMapping(button, i * ms_numControl + control );
    }
  }
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(slotButtonPressed(int)));
}

double GravityScene::getFps() const
{
  return fps;
}

void
GravityScene::step(unsigned int /*turn*/)
{
  m_environment->oneStep();
  QPointF centreOfMass = m_environment->calcCentreOfMass(m_environment->getCurrentTime());
  for (BodyItem* bodyItem : m_bodyItems)
  {
    bodyItem->updateItem(getTableRect(), centreOfMass);
  }
  updateTrackItems();
  handleCollisions();
  checkScores();
}

QPointF
GravityScene::envToScene(const QPointF& point, const QRectF& tableRect, const QPointF& centreOfMass)
{
  double sceneCentreOffsetX = tableRect.topLeft().x() + tableRect.width() / 2.0;
  double sceneCentreOffsetY = tableRect.topLeft().y() + tableRect.height() / 2.0;
  double scaleFactor = getScaleFactor(tableRect);
  double x = (point.x() - centreOfMass.x())/AU * scaleFactor + sceneCentreOffsetX;
  double y = (point.y() - centreOfMass.y())/AU * scaleFactor + sceneCentreOffsetY;
  QPointF pointScene(x, y);
  return pointScene;
}

QPointF
GravityScene::sceneToEnv(const QPointF& point, const QRectF& tableRect, const QPointF& centreOfMass)
{
  double sceneCentreOffsetX = tableRect.topLeft().x() + tableRect.width() / 2.0;
  double sceneCentreOffsetY = tableRect.topLeft().y() + tableRect.height() / 2.0;
  double scaleFactor = getScaleFactor(tableRect);
  double x = (point.x() - centreOfMass.x() - sceneCentreOffsetX) * AU / scaleFactor;
  double y = (point.y() - centreOfMass.y() - sceneCentreOffsetY) * AU / scaleFactor;
  return QPointF(x, y);
}

double
GravityScene::getScaleFactor(const QRectF& tableRect)
{
  return std::min(tableRect.width(), tableRect.height()) * zoom;
}

void GravityScene::slotButtonPressed(int b)
{
  unsigned int spaceshipId = b/ms_numControl;
  m_environment->boost(spaceshipId, static_cast<Body::Direction>( b%ms_numControl ));
  m_spaceships[spaceshipId]->setShowRocketOn();
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
      QPointF com = m_environment->calcCentreOfMass(time);
      QLineF newLine(envToScene(QPointF(s2[0], s2[1]), getTableRect(), com), envToScene(QPointF(s1[0], s1[1]), getTableRect(), com));
      QGraphicsLineItem* line = addLine(newLine, QPen(body->getTrackColor()));
      line->setZValue(-10);
      lines.push_back(line);
    }
  }
}

void
GravityScene::createCelestialBodies()
{
  Body* sun = new Body(0.0, 0.0, 0.0, 0.0, sunMass, Qt::yellow, m_environment.get());
  addPlanet(sun, Qt::yellow);

  double earthEccentricity = 0.0167086;
  const double earthMass = 5.972e24;
  double earthX = 152.10e9 * 1.2;  // * 1.4 to create larger distance
  double earthVy = Environment::calcOrbitalVelocity(earthX, earthEccentricity, sunMass);
  Body* earth = new Body(earthX, 0.0, 0.0, -earthVy, earthMass, Qt::white, m_environment.get());
  addPlanet(earth, Qt::green);

  double venusEccentricity = 0.006772;
  const double venusMass = 4.8675e24;
  double venusX = 108.939e9;
  double venusVy = Environment::calcOrbitalVelocity(venusX, venusEccentricity, sunMass);
  Body* venus = new Body(venusX, 0.0, 0.0, -venusVy, venusMass, Qt::white, m_environment.get());
  addPlanet(venus, Qt::blue);

  double mercuryEccentricity = 0.205630;
  const double mercuryMass = 3.3011e23;
  double mercuryX = 69.81690e9;
  double mercuryVy = Environment::calcOrbitalVelocity(mercuryX, mercuryEccentricity, sunMass);
  Body* mercury = new Body(mercuryX, 0.0, 0.0, -mercuryVy, mercuryMass, Qt::white, m_environment.get());
  addPlanet(mercury, Qt::red);

//  double marsEccentricity = 0.0934;
//  const double massMars = 6.4171e23;
//  double marsX = 227.9e9;
//  double marsY = Environment::calcOrbitalVelocity(marsX, marsEccentricity, sunMass);
//  Body* mars = new Body(marsX, 0.0, 0.0, -marsY, massMars, Qt::white, m_environment.get());
//  addPlanet(mars, Qt::red);
}

void
GravityScene::createSpaceShips()
{
  const double s1X = 100e9;
  const double s1Vy = Environment::calcOrbitalVelocity(s1X, 0, sunMass);
  std::array<QColor, 3> colors{QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue)};
  for ( unsigned int i = 0; i != getNumPlayers(); ++i )
  {
    double angle = 2 * M_PI * i / getNumPlayers();
    double x = sin(angle) * s1X;
    double y = cos(angle) * s1X;
    double vx = cos(angle) * s1Vy;
    double vy = -sin(angle) * s1Vy;
    Body* s1 = new Body(x, y, vx, vy, 0, colors[i%3], m_environment.get());
    addSpaceship(s1, i);
  }
}

void
GravityScene::createCargo(Planet* notOnMe)
{
  std::uniform_int_distribution<> distribution(1, m_planets.size()-1);

  std::size_t indexA = distribution(m_randomGenerator);
  while (m_planets[indexA] == notOnMe)
  {
    indexA = distribution(m_randomGenerator);
  }
  std::size_t indexB = distribution(m_randomGenerator);
  while (indexA == indexB)
  {
    indexB = distribution(m_randomGenerator);
  }
  Cargo* cargo = new Cargo(m_planets[indexA], m_planets[indexB]);
  m_bodyItems.push_back(cargo);
  m_cargos.push_back(cargo);
  addItem(cargo);
}

void
GravityScene::handleCollisions()
{
  std::vector<Cargo*> toRemoveCargos;
  for (const auto& spaceshipId : m_spaceships)
  {
    auto spaceship = spaceshipId.second;
    for (const auto& planet : m_planets)
    {
      if (!spaceship->collidesWithItem(planet))
      {
        continue;
      }
      for (const auto& cargo : m_cargos)
      {
        Cargo::Action action = cargo->notifyCollision(spaceship, planet);
        switch (action)
        {
          case Cargo::Action::pickup :
          {
            createCargo(planet);
            break;
          }
          case Cargo::Action::dropoff :
          {
            unsigned int playerId = spaceship->getPlayerId();
            m_scores[playerId] += 10;
            setScore(playerId, m_scores[playerId]);
            toRemoveCargos.push_back(cargo);
            break;
          }
          case Cargo::Action::none :
            break;
        }
      }
    }
  }

  for (auto cargo : toRemoveCargos)
  {
    m_cargos.erase( std::remove(m_cargos.begin(), m_cargos.end(), cargo), m_cargos.end() );
    removeBodyItem(cargo);
    delete cargo;
  }
}

void GravityScene::removeBodyItem(BodyItem* bodyItem)
{
  removeItem(bodyItem);
  m_bodyItems.erase( std::remove(m_bodyItems.begin(), m_bodyItems.end(), bodyItem), m_bodyItems.end() );
}

void GravityScene::checkScores()
{
  for (const auto& score : m_scores)
  {
    if (score.second >= winningScore)
    {
      QFont font;
      int spaceshipWidth = getTableRect().width() * 0.3;
      int textSize = getTableRect().width() * 0.1;
      font.setPixelSize(textSize);
      addText("WINNER", font);
      m_spaceships[score.first]->setSize(spaceshipWidth);
    }
  }
}
