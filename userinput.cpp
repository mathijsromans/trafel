#include "userinput.h"
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <array>
#include <cassert>
#include <ctime>
#include <unistd.h>
#include <unordered_set>

#define USE_CAMERA 0

#if USE_CAMERA
#include <raspicam/raspicam.h>

namespace
{

raspicam::RaspiCam& getCam()
{
  static raspicam::RaspiCam camera;
  static bool initialised = false;
  if ( !initialised )
  {
    initialised = true;
    qDebug() << "Opening camera..." << endl;
    if ( !camera.open() )
    {
      exit(1);
    }

    camera.setExposure(raspicam::RASPICAM_EXPOSURE_AUTO);
    camera.setISO(100); // note: 100 to 800
    camera.setShutterSpeed(10000);

    //wait a while until camera stabilizes
    usleep(3000000);
  }

  //capture
  return camera;
}

}
#endif


UserInput::UserInput()
  : m_testImage(QImage("../trafel/test_0.png")),
    m_currentImage()
{
  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotCheck()));
  timer->start(2000);
  QTimer::singleShot(0, this, SLOT(slotCheck()));
}

UserInput::~UserInput()
{
}

void UserInput::getImage()
{
  QTime time;
  time.start();

#if USE_CAMERA
  raspicam::RaspiCam& camera = getCam();
  camera.grab();
  m_currentImage.setSize( camera.getWidth(), camera.getHeight(), camera.getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB ) );
  camera.retrieve( m_currentImage.getData(), raspicam::RASPICAM_FORMAT_RGB );
#else
  m_currentImage = m_testImage;
#endif

  qDebug() << "getImage() took" << time.elapsed() << "ms";
}

namespace
{
}

bool UserInput::searchLine(int y, std::array<QPoint,3>& result, std::array<int, 3>& bestScore) const
{
  bool positiveScore = false;
  const int width = m_currentImage.getWidth();
  Score s;
  const Image::RGB* line = m_currentImage.scanLine(y);
  for ( int x = 0; x < width; ++x )
  {
    s.addPoint(line[x]);
    for ( unsigned int c = 0; c < 3; ++c )
    {
      int score = s.getScore(static_cast<Score::Color>(c));
      if ( score > 0 )
      {
        positiveScore = true;
      }
      if ( score > bestScore[c] )
      {
        bestScore[c] = score;
        result[c] = QPoint(x - 3*ds/2, y);
        //          qDebug() << "new best point for color " << c <<
        //                      " at " << result[c] <<
        //                      " with scores: " <<
        //                      s.scoreBright << "," <<
        //                      s.scoreColor[0] << "," <<
        //                      s.scoreColor[1] << "," <<
        //                      s.scoreColor[2] << ":" <<
        //                      score;
      }
    }
  }
  return positiveScore;
}

std::array<QPoint,3> UserInput::getPointer() const
{
  QTime time;
  time.start();

  std::array<QPoint,3> result = {QPoint(0,0), QPoint(0,0), QPoint(0,0)};

  std::array<int, 3> bestScore;
  bestScore.fill(0);
  const int height = m_currentImage.getHeight();
  const int stepSize = 4;
  std::unordered_set<int> moreLinesToBeSearched;
  for ( int y = 0; y < height; y += stepSize )
  {
    if( searchLine(y, result, bestScore) )
    {
      for ( int i = y-stepSize+1; i < y+stepSize; ++i )
      {
        moreLinesToBeSearched.insert(i);
      }
    }
  }
  for ( int y : moreLinesToBeSearched )
  {
    searchLine(y, result, bestScore);
  }

  qDebug() << "getPoint() took" << time.elapsed() << "ms : " << result[0] << ", " << result[1] << ", " << result[2];
  return result;
}

void UserInput::slotCheck()
{
  getImage();
  QImage image = m_currentImage.toImage();
  signalNewImage(image);

  static int counter = 0;
  QString fileName = QString("grab_") + QString::number(counter++) + ".png";
  image.save( fileName );
//  qDebug() << "saved to " << fileName;

  std::array<QPoint,3> p = getPointer();
  if ( p[0] != QPoint(0,0) )
  {
    signalMouseClick(p[0]);
  }
}


UserInput::Image::Image(const QImage& image)
  : m_width(image.width()),
    m_height(image.height()),
    m_data(m_width*m_height)
{
  for ( unsigned int y = 0; y < m_height; ++y )
  {
    for ( unsigned int x = 0; x < m_width; ++x )
    {
      QRgb c = image.pixel( x, y );
      m_data[y * m_width + x] = RGB{ static_cast<unsigned char>(qRed(c)),
                                     static_cast<unsigned char>(qGreen(c)),
                                     static_cast<unsigned char>(qBlue(c)) };
    }
  }
}

void UserInput::Image::setSize(unsigned int width, unsigned int height, unsigned int dataSize)
{
  if ( dataSize != sizeof(RGB)*width*height)
  {
    throw std::logic_error("incorrect image byte size");
  }
  m_width = width;
  m_height = height;
  m_data.resize(m_width*m_height);
}

QImage UserInput::Image::toImage() const
{
  return QImage(getConstData(), m_width, m_height, QImage::Format_RGB888).copy();
}

const UserInput::Image::RGB* UserInput::Image::scanLine(unsigned int y) const
{
  return &m_data[y * m_width];
}

void UserInput::Score::addPoint(UserInput::Image::RGB c)
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

UserInput::Score::Score() : pixels(), scoreBright(0), scoreColor(), oldestPixel(0)
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

int UserInput::Score::brightScore(UserInput::Image::RGB c)
{
  return static_cast<int>(c.r) +
      static_cast<int>(c.g) +
      static_cast<int>(c.b) - 3*128;
}

int UserInput::Score::redScore(UserInput::Image::RGB c)
{
  return 2 * c.r - c.g - c.b;
}

int UserInput::Score::greScore(UserInput::Image::RGB c)
{
  return 2 * c.g - c.r - c.b;
}

int UserInput::Score::bluScore(UserInput::Image::RGB c)
{
  return 2 * c.b - c.r - c.g;
}

int UserInput::Score::getScore( Color c ) const
{
  auto scoreC = scoreColor[static_cast<unsigned int>(c)];
  if ( scoreBright <= 0 ||
       scoreC <= 0 )
  {
    return 0;
  }
  return scoreBright + scoreC;
}
