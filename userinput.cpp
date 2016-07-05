#include "userinput.h"
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <array>
#include <cassert>
#include <ctime>
#include <unistd.h>

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

std::experimental::optional<QPoint> UserInput::getPointer() const
{
  QTime time;
  time.start();

  std::experimental::optional<QPoint> result;

  const int height = m_currentImage.getHeight();
  const int width = m_currentImage.getWidth();
  if ( height < ds ||
       width < ds )
  {
    return result;
  }

  static int counter = 0;
  QFile file("check"+QString::number(counter++) + ".txt");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return result;
  QTextStream out(&file);

  int bestScore = -1;
  Score s;
  for ( int y = 0; y < height/10; ++y )
  {
    const Image::RGB* line = m_currentImage.scanLine(y);
    for ( int x = 0; x < width/10; ++x )
    {
      s.addPoint(line[x], QPoint(x,y), out);
      if ( s.getScore() > bestScore )
      {
        bestScore = s.getScore();
        result = QPoint(x - ds/2, y);
        qDebug() << "new best point at " << *result << " with score " << s.scoreA << ", " << s.scoreC << " : " << s.getScore();
        if ( result == QPoint(459,802) )
        {
            qDebug() << "hello";
        }
      }
    }
  }
  qDebug() << "getPoint() took" << time.elapsed() << "ms : " << (result ? *result : QPoint(0,0));
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

  std::experimental::optional<QPoint> p = getPointer();
  if ( p )
  {
    signalMouseClick(*p);
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

UserInput::ScoreOnePixel::ScoreOnePixel(UserInput::Image::RGB c, QPoint p, QTextStream &s)
  : color(c)
{
    scoreA = c.r + c.g + c.b - 3*128;
    scoreB = 0;
    scoreC = 2 * c.r - c.g - c.b;
    s << p.x() << "," << p.y() << ":" <<
         c.r << "," << c.g << "," << c.b << '\n';
}

void UserInput::Score::addPoint(UserInput::Image::RGB c, QPoint p, QTextStream &s)
{
    ScoreOnePixel& sp = scores[newScoreIndex];
    scoreA -= sp.scoreA;
    scoreB -= sp.scoreB;
    scoreC -= sp.scoreC;
    sp = ScoreOnePixel(c, p, s);
    scoreA += sp.scoreA;
    scoreB += sp.scoreB;
    scoreC += sp.scoreC;

    newScoreIndex = (newScoreIndex+1)%ds;
}
