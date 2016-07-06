#include "scorekeeper.h"

ScoreKeeper::ScoreKeeper() : pixels(), scoreBright(0), scoreColor(), oldestPixel(0)
{
  Image::RGB zeroPixel{0,0,0};
  pixels.fill(zeroPixel);
  scoreColor.fill(0);
  for ( unsigned int i = 0; i < ds; ++i )
  {
    scoreColor[0] += 2*redScore(zeroPixel);
    scoreColor[1] += 2*greScore(zeroPixel);
    scoreColor[2] += 2*bluScore(zeroPixel);
    scoreBright   += brightScore(zeroPixel);
  }
}

void ScoreKeeper::addPoint(Image::RGB c)
{
  scoreColor[0] -= redScore(pixels[oldestPixel]);
  scoreColor[1] -= greScore(pixels[oldestPixel]);
  scoreColor[2] -= bluScore(pixels[oldestPixel]);
  unsigned int oneThird = oldestPixel + ds;
  if ( oneThird >= 3*ds ) { oneThird -= 3*ds; }
  scoreBright -= brightScore(pixels[oneThird]);
  scoreColor[0] += redScore (pixels[oneThird]);
  scoreColor[1] += greScore (pixels[oneThird]);
  scoreColor[2] += bluScore (pixels[oneThird]);
  unsigned int twoThird = oneThird + ds;
  if ( twoThird >= 3*ds ) { twoThird -= 3*ds; }
  scoreBright += brightScore(pixels[twoThird]);
  scoreColor[0] -= redScore (pixels[twoThird]);
  scoreColor[1] -= greScore (pixels[twoThird]);
  scoreColor[2] -= bluScore (pixels[twoThird]);
  pixels[oldestPixel] = c;
  scoreColor[0] += redScore(c);
  scoreColor[1] += greScore(c);
  scoreColor[2] += bluScore(c);
  ++oldestPixel;
  if ( oldestPixel == 3*ds ) { oldestPixel = 0; }
}

int ScoreKeeper::getScore( Color c ) const
{
  auto scoreC = scoreColor[static_cast<unsigned int>(c)];
  if ( scoreBright <= 0 ||
       scoreC <= 0 )
  {
    return 0;
  }
  return scoreBright + scoreC;
}
