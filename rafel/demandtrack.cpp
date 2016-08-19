#include "demandtrack.h"
#include "rafelitem.h"
#include "rafelscene.h"
#include "utilities.h"

namespace
{
  unsigned int queueLength = 5;
}

DemandTrack::DemandTrack(RafelScene& scene, QPointF pos)
 : m_scene(scene),
   m_pos(pos)
{
  fillQueue();
  positionItems();
}

void DemandTrack::fillQueue()
{
  double sz = 0.2*m_scene.itemSize();
  while (m_queue.size() < queueLength)
  {
    std::vector<RafelItem*> items;
    unsigned int amount = Utilities::getRnd(1, 4);
    RafelItem::Type type = static_cast<RafelItem::Type>( Utilities::getRnd(0, static_cast<unsigned int>(RafelItem::Type::MAX) ) );
    for ( unsigned int j = 0; j != amount; ++j )
    {
      auto item = new RafelItem(sz, type);
      m_scene.addItem(item);
      items.push_back(item);
    }
    m_queue.push_back(items);
  }
}

void DemandTrack::positionItems()
{
  double sz1 = 0.3*m_scene.itemSize();
  double sz2 = 0.2*m_scene.itemSize();
  for ( unsigned int i = 0; i != m_queue.size(); ++i )
  {
    for ( unsigned int j = 0; j != m_queue[i].size(); ++j )
    {
      m_queue[i][j]->setPos(m_pos + QPointF(i*sz1 , j*sz2));
    }
  }
}

