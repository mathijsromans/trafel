#ifndef USERINPUT_H
#define USERINPUT_H

#include "image.h"
#include "pointerevent.h"
#include <array>
#include <QImage>
#include <QObject>

class QTextStream;

class UserInput : public QObject
{
  Q_OBJECT

public:
  UserInput();
  virtual ~UserInput();
  void process();
signals:
  void signalEvent(PointerEvent point);
private:
  void getImage(unsigned int cameraISO);
  PointerEvent getEvent() const;
  bool searchLine(int y, std::array<QPoint, 3>& result, std::array<int, 3>& bestScore) const;
private:  
  Image m_testImage;
  Image m_currentImage;
  PointerEvent m_lastEvent;
};

#endif // USERINPUT_H
