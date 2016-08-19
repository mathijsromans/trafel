#ifndef DEMANDTRACK_H
#define DEMANDTRACK_H

#include <deque>
#include <vector>
#include <QPointF>
#include "rafelitem.h"
class RafelScene;

class DemandTrack
{
public:
  explicit DemandTrack(RafelScene& scene, QPointF pos);
  std::array<int, static_cast<unsigned int>(RafelItem::Type::MAX)> getDemand() const;
private:
  void fillQueue();
  void positionItems();
private:
  RafelScene& m_scene;
  QPointF m_pos;
  std::deque<std::vector<RafelItem*>> m_queue;
};

#endif // DEMANDTRACK_H
