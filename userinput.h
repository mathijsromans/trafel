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
  void getImage();
signals:
  void signalNewImage(QImage image);
  void signalMouseClick(PointerEvent point);
private slots:
  void slotCheck();
private:
  std::array<QPoint, 3> getPointer() const;
  bool searchLine(int y, std::array<QPoint, 3>& result, std::array<int, 3>& bestScore) const;
private:  
  Image m_testImage;
  Image m_currentImage;
};

#endif // USERINPUT_H
