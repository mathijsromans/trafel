#ifndef RAFELSCENE_H
#define RAFELSCENE_H

#include "transformscene.h"
class DemandTrack;
class Train;
class Button;

class RafelScene : public TransformScene
{
  Q_OBJECT

public:
  RafelScene();
  ~RafelScene() override;
  double getFps() const;
  double itemSize() const;
  QPointF position(double angle) const;
private:
  void init() override;
  void eventClick(QPointF p, PointerEvent::Color c) override;
  void step(unsigned int turn);
  QPointF stationPosition(unsigned int nr) const;
  double stationAngle(unsigned int nr) const;
  double angleStep() const;
private slots:
  void slotGoButtonPressed( int player );
private:
  enum class Status {driving, stopped};
  Status m_status;
  unsigned int m_atStation;
  std::unique_ptr<Train> m_train;
  std::vector<std::unique_ptr<DemandTrack>> m_demands;
  std::vector<Button*> m_goButtons;
};

#endif // RAFELSCENE_H
