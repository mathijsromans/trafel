#ifndef TRAIN_H
#define TRAIN_H

class PixmapItem;
class RafelItem;
class DemandTrack;
class RafelScene;
#include <vector>

class Train
{
public:
  explicit Train(RafelScene& scene);
  ~Train();
  void setAngle(double angle);
  void addItem(RafelItem* item, unsigned int wagon);
  double angle() const;
  void exchangeWith(DemandTrack& track);

private:
  struct Wagon
  {
    PixmapItem* wagon;
    std::vector<RafelItem*> items;
  };

  RafelScene& m_scene;
  double m_angle;
  PixmapItem* m_train;
  std::vector<Wagon> m_wagons;
};

#endif // TRAIN_H
