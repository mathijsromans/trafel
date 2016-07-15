#ifndef SOLARSYSTEMSCENE_H
#define SOLARSYSTEMSCENE_H

#include "transformscene.h"

#include <QObject>

#include <memory>

class Environment;
class Body;
class BodyItem;
class QTimer;

class SolarSystemScene : public TransformScene
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
  explicit SolarSystemScene();
  virtual ~SolarSystemScene();

  virtual void init() override;

  static QPointF envToScene(const QPointF& point, const QRectF& tableRect);
  static QPointF sceneToEnv(const QPointF& point, const QRectF& tableRect);


protected:
  virtual void mouseClick(QPointF point) override;

private slots:
  void step();

private:
  BodyItem* addBody(Body* body, const QColor& color=Qt::white);
  void detectCollisionWithSun();
  void removeBodyItem(BodyItem* bodyItem);
  void createCelestialBodies();
  static double getScaleFactor(const QRectF& tableRect);

private:
  std::unique_ptr<Environment> m_environment;
  std::vector<BodyItem*> m_bodyItems;
  std::vector<Body*> m_bodies;
  std::unique_ptr<NewBodyData> m_newBody;
  QGraphicsEllipseItem* m_tempBodyItem;
  BodyItem* m_sunItem;
  QTimer* m_timer;
};

#endif // SOLARSYSTEMSCENE_H
