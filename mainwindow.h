#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calibration.h"
#include "mainmenuscene.h"
#include "pointerevent.h"
#include "userinputthread.h"
#include <QMainWindow>
#include <memory>
class QGraphicsView;
class TrafficScene;
class TransformScene;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected:
  virtual void showEvent(QShowEvent* event) override;
  virtual void resizeEvent(QResizeEvent* event) override;

private slots:
  void slotStartGame(MainMenuScene::Games game);
  void slotSetNumPlayers(int numPlayers);
  void slotInputEvent(PointerEvent e);
  void slotQuit();

private:
  void fitInView() const;

private:
  UserInputThread m_userInputThread;
  QGraphicsView* v;
  std::unique_ptr<TransformScene> m_currentScene;
  Calibration m_calibration;
  unsigned int m_numPlayers = 3;
};

#endif // MAINWINDOW_H
