#ifndef EARTH_H
#define EARTH_H

#include <vector>

class Earth
{
public:
  Earth( double dx, double dy);
  double height(double x, double y) const;
private:
  struct Mountain
  {
    double height;
    double width;
    double x;
    double y;
  };

  std::vector<Mountain> m_mountains;
};

#endif // EARTH_H
