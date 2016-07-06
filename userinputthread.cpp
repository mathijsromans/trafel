#include "userinputthread.h"
#include "userinput.h"
#include <QDebug>

UserInputThread::UserInputThread()
  : QThread(),
    m_mutex(),
    m_abort(false),
    m_ui(0)
{
}

void UserInputThread::setUI(UserInput* ui)
{
  m_ui = ui;
  m_ui->moveToThread(this);
}

UserInputThread::~UserInputThread()
{
  qDebug() << "quitting thread";
  m_mutex.lock();
  m_abort = true;
  m_mutex.unlock();
  qDebug() << "waiting for thread";
  wait();
}

void UserInputThread::run()
{
  while( !m_abort )
  {
    m_ui->process();
  }
}

