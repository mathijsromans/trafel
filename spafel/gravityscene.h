#ifndef GRAVITYSCENE_H
#define GRAVITYSCENE_H

#include "transformscene.h"

#include <QObject>

#include <deque>
#include <memory>

class Environment;
class Body;
class BodyItem;
class Planet;
class QTimer;

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

protected:
  void eventClick(QPointF point, PointerEvent::Color c) override;

private slots:
  void step();

private:
  virtual void init() override;
  void addPlanet(Body* body, const QColor& lineColor);
  void addSpaceship(Body* body, unsigned int id);
  void updateTrackItems();
  void createCelestialBodies();
  void createSpaceShips();
  void createCargo();
  static double getScaleFactor(const QRectF& tableRect);

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
  std::unique_ptr<NewBodyData> m_newBody;
  std::map<Body*, std::deque<QGraphicsLineItem*>> m_trackItems;
  QGraphicsEllipseItem* m_tempPlanet;
  QTimer* m_timer;
};

#endif // GRAVITYSCENE_H
