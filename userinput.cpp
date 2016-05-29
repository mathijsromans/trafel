#include "userinput.h"
#include <QTime>
#include <QTimer>
#include <QDebug>
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
  timer->start(50);
}

UserInput::~UserInput()
{
}

QImage UserInput::getImage()
{
#if USE_CAMERA

  raspicam::RaspiCam& camera = getCam();
  camera.grab();

  //allocate memory
  delete[] m_imageData;
  m_imageData = new unsigned char[ camera.getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
  //extract the image in rgb format
  camera.retrieve( m_imageData, raspicam::RASPICAM_FORMAT_RGB );
  QImage image(m_imageData, camera.getWidth(), camera.getHeight(), QImage::Format_RGB888);
  return image;
#endif


  return m_testImage;

}

std::experimental::optional<QPoint> UserInput::getPointer(const QImage& image)
{
//  if ( qrand() % 2 )
//  {
//    return QPoint(qrand() % image.width(),
//                  qrand() % image.height() );
//  }

  // find 3x3 pixels which are very white and slightly reddish
  const int ds = 3;
  int bestScore = -1;
  std::experimental::optional<QPoint> result;
  for ( int x = ds/2; x < image.width()-ds/2; ++x )
  {
    for ( int y = ds/2; y < image.height()-ds/2; ++y )
    {
      for ( int dx = x - ds/2; dx <= x + ds/2; ++dx)
      {
        for ( int dy = y - ds/2; dy <= y + ds/2; ++dy)
        {
          QPoint p(dx, dy);
          QRgb c = image.pixel(p);
          int scoreA = qRed(c) - 250;
          int scoreB = qRed(c) - std::min(255, qGreen(c)+1);
          int scoreC = qRed(c) - std::min(255, qBlue(c)+1);
          int sum = scoreA + scoreB + scoreC;
          if ( scoreA >= 0 &&
               scoreB >= 0 &&
               scoreC >= 0 &&
               sum > bestScore )
          {
            bestScore = sum;
            result = p;
          }
        }
      }
    }
  }
  return result;
}

void UserInput::slotCheck()
{
  QTime time;
  time.start();
  QImage image = getImage();

  static int counter = 0;
  image.save( QString( "grab_" + QString::number(counter++) + ".jpg" ) );

  std::experimental::optional<QPoint> p = getPointer(image);
  if ( p )
  {
    signalMouseClick(*p);
  }
//  qDebug() << time.elapsed();
}


