#ifndef DEMANDTRACK_H
#define DEMANDTRACK_H

#include <deque>
#include <vector>
#include <QPointF>
class RafelItem;
class RafelScene;

class DemandTrack
{
public:
  explicit DemandTrack(RafelScene& scene, QPointF pos);
private:
  void fillQueue();
  void positionItems();
private:
  RafelScene& m_scene;
  QPointF m_pos;
  std::deque<std::vector<RafelItem*>> m_queue;
};

#endif // DEMANDTRACK_H
