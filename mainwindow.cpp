#include "mainwindow.h"
#include "trafficscene.h"
#include "transformscene.h"
#include "userinput.h"
#include "mainmenuscene.h"
#include "flafel/flafelscene.h"
#include "rafel/rafelscene.h"
#include "spafel/gravityscene.h"
#include "poker/pokerscene.h"
#include <QCoreApplication>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <cassert>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_userInputThread(),
  m_calibration()
{
  qRegisterMetaType<PointerEvent>();

  v = new QGraphicsView();
  setCentralWidget(v);
//  showMaximized();
  showFullScreen();

  UserInput* ui = new UserInput();
  connect(ui, SIGNAL(signalEvent(PointerEvent)), this, SLOT(slotInputEvent(PointerEvent)));

  m_userInputThread.setUI(ui);
  m_userInputThread.start();

  slotStartGame( MainMenuScene::Games::mainMenu );
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

void MainWindow::slotInputEvent(PointerEvent e)
{
  m_calibration.processEvent(e);
}

void MainWindow::slotQuit()
{
  if ( dynamic_cast<MainMenuScene*>( m_currentScene.get() ) )
  {
    QCoreApplication::exit();
  }
  else
  {
    slotStartGame(MainMenuScene::Games::mainMenu);
  }
}

void MainWindow::fitInView() const
{
  v->setSceneRect(QRectF(0,0,1000,800));
}

void MainWindow::slotStartGame(MainMenuScene::Games game)
{
  if ( m_currentScene )
  {
    m_currentScene->deleteLater();
    m_currentScene.release();
  }

  typedef MainMenuScene::Games Games;
  switch (game)
  {
    case Games::mainMenu:
    {
      auto mainMenuScene = std::make_unique<MainMenuScene>();
      connect(mainMenuScene.get(), SIGNAL(signalStartGame(MainMenuScene::Games)), this, SLOT(slotStartGame(MainMenuScene::Games)));
      m_currentScene = std::move(mainMenuScene);
    }
    break;
    case Games::poker:  m_currentScene = std::make_unique<PokerScene>(); break;
    case Games::trafel: m_currentScene = std::make_unique<TrafficScene>(); break;
    case Games::spafel: m_currentScene = std::make_unique<GravityScene>(); break;
    case Games::flafel: m_currentScene = std::make_unique<FlafelScene>(); break;
    case Games::rafel:  m_currentScene = std::make_unique<RafelScene>(); break;
    case Games::MAX: assert(false);
  }
  m_calibration.setScene(m_currentScene.get());
  v->setScene(m_currentScene.get());
  connect(m_currentScene.get(), SIGNAL(signalMouseEvent(PointerEvent)), this, SLOT(slotInputEvent(PointerEvent)));
  connect(m_currentScene.get(), SIGNAL(signalQuit()), this, SLOT(slotQuit()));
}
