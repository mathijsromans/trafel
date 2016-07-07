#ifndef SCOREKEEPER_H
#define SCOREKEEPER_H

#include "image.h"
#include <array>

class ScoreKeeper
{
public:
  enum class Color{ red, green, blue };
  explicit ScoreKeeper();
  void addPoint( Image::RGB c );
  int getScore( Color c ) const;
  static const int ds = 5;
private:
  static int brightScore( Image::RGB c ) { return static_cast<int>(c.r) +
                                                  static_cast<int>(c.g) +
                                                  static_cast<int>(c.b) - 3*100; }
  static int redScore( Image::RGB c ) { return 2 * c.r - c.g - c.b - brightScore(c); }
  static int greScore( Image::RGB c ) { return 2 * c.g - c.r - c.b - brightScore(c); }
  static int bluScore( Image::RGB c ) { return 2 * c.b - c.r - c.g - brightScore(c); }
private:
  std::array<Image::RGB, 3*ds> pixels;
  int scoreBright;
  std::array<int, 3> scoreColor;
  unsigned int oldestPixel;
};

#endif // SCOREKEEPER_H
