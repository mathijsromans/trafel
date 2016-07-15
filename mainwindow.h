#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "userinputthread.h"
#include <QMainWindow>
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
  void slotQuit();

private:
  void fitInView() const;

private:
  UserInputThread m_userInputThread;
  QGraphicsView* v;
  TransformScene* t;
};

#endif // MAINWINDOW_H
