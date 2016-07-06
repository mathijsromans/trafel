#ifndef USERINTERFACETHREAD_H
#define USERINTERFACETHREAD_H

#include <QMutex>
#include <QThread>
class UserInput;

class UserInputThread : public QThread
{

  Q_OBJECT

public:
  UserInputThread();
  void setUI( UserInput* ui );
  virtual ~UserInputThread();

protected:
  void run() override;
private:
  QMutex m_mutex;
  bool m_abort;
  UserInput* m_ui;
};

#endif // USERINTERFACETHREAD_H
