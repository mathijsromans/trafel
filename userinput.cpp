#include "userinput.h"
#include "scorekeeper.h"
#include <QFile>
#include <QTime>
#include <QDebug>
#include <array>
#include <cassert>
#include <unistd.h> // for usleep
#include <unordered_set>

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

    camera.setExposure(raspicam::RASPICAM_EXPOSURE_AUTO);
    camera.setISO(100); // note: 100 to 800
    camera.setShutterSpeed(5000);

    //wait a while until camera stabilizes
    usleep(3000000);
  }

  //capture
  return camera;
}

}
#endif


UserInput::UserInput()
  : m_testImage(),
    m_currentImage()
{
}

UserInput::~UserInput()
{
}

void UserInput::getImage()
{
//  QTime time;
//  time.start();

#if USE_CAMERA
  raspicam::RaspiCam& camera = getCam();
  camera.grab();
  m_currentImage.setSize( camera.getWidth(), camera.getHeight(), camera.getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB ) );
  camera.retrieve( m_currentImage.getData(), raspicam::RASPICAM_FORMAT_RGB );
#else
  m_currentImage = m_testImage;
#endif

//  qDebug() << "getImage() took" << time.elapsed() << "ms";
}

namespace
{
}

bool UserInput::searchLine(int y, std::array<QPoint,3>& result, std::array<int, 3>& bestScore) const
{
  bool positiveScore = false;
  const int width = m_currentImage.getWidth();
  ScoreKeeper s;
  const Image::RGB* line = m_currentImage.scanLine(y);
  for ( int x = 0; x < width; ++x )
  {
    s.addPoint(line[x]);
    for ( unsigned int c = 0; c < 3; ++c )
    {
      int score = s.getScore(static_cast<ScoreKeeper::Color>(c));
      if ( score > 0 )
      {
        positiveScore = true;
      }
      if ( score > bestScore[c] )
      {
        bestScore[c] = score;
        result[c] = QPoint(x - 3*ScoreKeeper::ds/2, y);
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

PointerEvent UserInput::getEvent() const
{
//  QTime time;
//  time.start();

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

//  qDebug() << "getEvent() took" << time.elapsed() << "ms : " << result[0] << ", " << result[1] << ", " << result[2];
  return PointerEvent({result[0], result[1], result[2]});
}

void UserInput::process()
{
//  QTime time;
//  time.start();

  getImage();

//  static int counter = 0;
//  QString fileName = QString("grab_") + QString::number(counter++) + ".png";
//  m_currentImage.save( fileName );
//  qDebug() << "saved to " << fileName;

  PointerEvent event( getEvent() );
  if ( event.differsFrom( m_lastEvent ) )
  {
    if ( !event.getAny().isNull() )
    {
      signalMouseClick(event);
    }
    m_lastEvent = event;
  }

//  qDebug() << "User input loop took" << time.elapsed() << "ms";
}
