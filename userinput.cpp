#include "userinput.h"
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <array>
#include <cassert>
#include <ctime>
#include <unistd.h>


#define USE_CAMERA 1

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
    //wait a while until camera stabilizes
    usleep(3000000);

    camera.setExposure(raspicam::RASPICAM_EXPOSURE_AUTO);
    camera.setISO(800);
    camera.setShutterSpeed(330000);
  }

  //capture
  return camera;
}

}
#endif


UserInput::UserInput()
  : m_imageData(0)
{
  m_testImage.load("../trafel/out.jpg");
  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotCheck()));
  timer->start(200);
}

UserInput::~UserInput()
{
}

QImage UserInput::getImage()
{
  QTime time;
  time.start();

  QImage result;

#if USE_CAMERA
  raspicam::RaspiCam& camera = getCam();
  camera.grab();

  //allocate memory
  delete[] m_imageData;
  m_imageData = new unsigned char[ camera.getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
  //extract the image in rgb format
  camera.retrieve( m_imageData, raspicam::RASPICAM_FORMAT_RGB );
  result = QImage(m_imageData, camera.getWidth(), camera.getHeight(), QImage::Format_RGB888);
#else
  result = m_testImage;
#endif

  qDebug() << "getImage() took" << time.elapsed() << "ms";
  return result;
}

namespace
{
struct ScoreOnePixel
{
  ScoreOnePixel() : scoreA(0), scoreB(0), scoreC(0) {}
  explicit ScoreOnePixel( QRgb c )
  {
    scoreA = qRed(c)- 220;
    scoreB = 80 - std::abs(110 - qGreen(c));
    scoreC = 80 - std::abs(110 - qBlue(c));
  }
  int scoreA;
  int scoreB;
  int scoreC;
};

const int ds = 15;

struct Score
{

  explicit Score() : scores(), scoreA(0), scoreB(0), scoreC(0), newScoreIndex(0) {}

  void addPoint( QRgb c )
  {
    ScoreOnePixel& sp = scores[newScoreIndex];
    scoreA -= sp.scoreA;
    scoreB -= sp.scoreB;
    scoreC -= sp.scoreC;
    sp = ScoreOnePixel(c);
    scoreA += sp.scoreA;
    scoreB += sp.scoreB;
    scoreC += sp.scoreC;

    newScoreIndex = (newScoreIndex+1)%ds;
  }

  int getScore() const
  {
    if ( scoreA >= 0 &&
         scoreB >= 0 &&
         scoreC >= 0 )
    {
      return scoreA + scoreB + scoreC;
    }
    return -1;
  }

  std::array<ScoreOnePixel, ds> scores;
  int scoreA;
  int scoreB;
  int scoreC;
  unsigned int newScoreIndex;
};
}

std::experimental::optional<QPoint> UserInput::getPointer(const QImage& image)
{
//#if !USE_CAMERA
//  if ( qrand() % 2 )
//  {
//    usleep(1000000);
//    return QPoint(qrand() % image.width(),
//                  qrand() % image.height() );
//  }
//#endif

  // find 3x3 pixels which are very white and slightly reddish
  QTime time;
  time.start();


  int bestScore = -1;
  std::experimental::optional<QPoint> result;

  const int height = image.height();
  const int width = image.width();
  if ( height < ds ||
       width < ds )
  {
    return result;
  }

  Score s;
  for ( int y = 0; y < height; ++y )
  {
    const QRgb* line = (QRgb *)image.constScanLine(y);
    for ( int x = 0; x < width; ++x )
    {
      s.addPoint(line[x]);
      if ( s.getScore() > bestScore )
      {
        bestScore = s.getScore();
        result = QPoint(x - ds/2, y);
//        qDebug() << "new best point at " << *result << " with score " << bestScore;
      }
    }
  }
  qDebug() << "getPoint() took" << time.elapsed() << "ms : " << (result ? *result : QPoint(0,0));
  return result;
}

void UserInput::slotCheck()
{
  QImage image = getImage();
  signalNewImage(image);

  static int counter = 0;
  QString fileName = QString("grab_") + QString::number(counter++) + ".jpg";
  image.save( fileName );
  qDebug() << "saved to " << fileName;

  std::experimental::optional<QPoint> p = getPointer(image);
  if ( p )
  {
    signalMouseClick(*p);
  }
}


