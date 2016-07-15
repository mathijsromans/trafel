#include "mainwindow.h"
#include "trafficscene.h"
#include "transformscene.h"
#include "userinput.h"
#include "spafel/gravityscene.h"
#include <QCoreApplication>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_userInputThread()
{
  qRegisterMetaType<PointerEvent>();

  v = new QGraphicsView();
  t = new GravityScene();
//  t = new TrafficScene();
  v->setScene(t);
  setCentralWidget(v);
//  showMaximized();
  showFullScreen();

  UserInput* ui = new UserInput();
  connect(ui, SIGNAL(signalMouseClick(PointerEvent)), t, SLOT(slotLightAt(PointerEvent)));
  connect(t, SIGNAL(quit()), this, SLOT(slotQuit()));

  t->calibrate();

  m_userInputThread.setUI(ui);
  m_userInputThread.start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
  fitInView();
}

void MainWindow::resizeEvent(QResizeEvent* /*event*/)
{
  fitInView();
}

void MainWindow::slotQuit()
{
  QCoreApplication::exit();
}

void MainWindow::fitInView() const
{
  v->setSceneRect(QRectF(0,0,1000,800));
}
