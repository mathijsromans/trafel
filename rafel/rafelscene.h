#ifndef RAFELSCENE_H
#define RAFELSCENE_H

#include "transformscene.h"
class PixmapItem;

class RafelScene : public TransformScene
{
public:
  RafelScene();
private:
  void init() override;
  void eventClick(QPointF p, PointerEvent::Color c) override;
  void step(unsigned int turn);
  double getFps() const;
  double itemSize() const;
private:
  PixmapItem* m_train;
  std::vector<PixmapItem*> m_wagons;
};

#endif // RAFELSCENE_H
