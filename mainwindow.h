#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pointerevent.h"
#include "userinputthread.h"
#include <QMainWindow>
class QGraphicsView;
class TrafficScene;

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
  void slotLightAt(PointerEvent e);

private:
  void fitInView() const;

private:
  UserInputThread m_userInputThread;
  QGraphicsView* v;
  TrafficScene* t;
};

#endif // MAINWINDOW_H
