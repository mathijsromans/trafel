#ifndef GRAVITYSCENE_H
#define GRAVITYSCENE_H

#include "transformscene.h"

#include <QObject>

#include <deque>
#include <memory>

class Environment;
class Body;
class BodyItem;
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
  static QPointF envToScene(const QPointF& point, const QRectF& tableRect);
  static QPointF sceneToEnv(const QPointF& point, const QRectF& tableRect);

protected:
  void eventClick(QPointF point, PointerEvent::Color c) override;

private slots:
  void step();

private:
  virtual void init() override;
  void addBody(Body* body);
  void addSpaceship(Body* body, unsigned int id);
  void updateTrackItems();
  void createCelestialBodies();
  void createSpaceShips();
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
  std::unique_ptr<NewBodyData> m_newBody;
  std::map<Body*, std::deque<QGraphicsLineItem*>> m_trackItems;
  QGraphicsEllipseItem* m_tempPlanet;
  QTimer* m_timer;
};

#endif // GRAVITYSCENE_H
