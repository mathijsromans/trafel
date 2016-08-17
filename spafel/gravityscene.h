#ifndef GRAVITYSCENE_H
#define GRAVITYSCENE_H

#include "transformscene.h"

#include <QObject>

#include <deque>
#include <memory>

class Environment;
class Body;
class BodyItem;
class Cargo;
class Planet;
class Spaceship;

class GravityScene : public TransformScene
{
  Q_OBJECT

public:
  struct NewBodyData
  {
  public:
    NewBodyData(double mass, const QPointF& scenePos)
      : m_mass(mass), scenePos(scenePos) {}
    double m_mass;
    QPointF scenePos;
  };

public:
  explicit GravityScene();
  virtual ~GravityScene();
  static QPointF envToScene(const QPointF& point, const QRectF& tableRect, const QPointF& centreOfMass);
  static QPointF sceneToEnv(const QPointF& point, const QRectF& tableRect, const QPointF& centreOfMass);

private:
  virtual void init() override;
  virtual unsigned int getFps() const override;
  virtual bool showScore() const override { return true; }
  virtual void step() override;
  void handleCollisions();
  void addPlanet(Body* body, const QColor& lineColor);
  void addSpaceship(Body* body, unsigned int id);
  void updateTrackItems();
  void createCelestialBodies();
  void createSpaceShips();
  void createCargo(Planet* notOnMe=0);
  static double getScaleFactor(const QRectF& tableRect);
  void removeBodyItem(BodyItem* bodyItem);

private slots:
  void slotButtonPressed(int b);

private:
  enum class Control {up, down, left, right, MAX};
  struct ButtonInLayout
  {
    Control control;
    QPointF pos;
  };


private:
  static const unsigned int ms_numControl = static_cast<unsigned int>(Control::MAX);
  static const std::array<std::string, ms_numControl> ms_buttonTexts;
  std::unique_ptr<Environment> m_environment;
  std::vector<BodyItem*> m_bodyItems;
  std::vector<Planet*> m_planets;
  std::vector<Spaceship*> m_spaceships;
  std::vector<Cargo*> m_cargos;
  std::map<Body*, std::deque<QGraphicsLineItem*>> m_trackItems;
  QGraphicsEllipseItem* m_tempPlanet;
  QTimer* m_timer;
  std::mt19937 m_randomGenerator;
  std::map<unsigned int, int> m_scores;
};

#endif // GRAVITYSCENE_H
