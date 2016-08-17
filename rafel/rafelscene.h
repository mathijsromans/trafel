#ifndef RAFELSCENE_H
#define RAFELSCENE_H

#include "transformscene.h"

class RafelScene : public TransformScene
{
public:
  RafelScene();
private:
  void init() override;
  void eventClick(QPointF p, PointerEvent::Color c) override;
};

#endif // RAFELSCENE_H
