#ifndef USERINPUT_H
#define USERINPUT_H

#include <QImage>
#include <QObject>
#include <experimental/optional>

class UserInput : public QObject
{
  Q_OBJECT

public:
  UserInput();
  virtual ~UserInput();
  const QImage& getImage() const;
signals:
  void signalMouseClick(QPoint);
private slots:
  void slotCheck();
private:
  static std::experimental::optional<QPoint> getPointer(const QImage& image);
  QImage m_testImage;
};

#endif // USERINPUT_H
