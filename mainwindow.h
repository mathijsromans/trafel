#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userinput.h"
class QGraphicsView;
class TableScene;
class QLabel;
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
  void slotMoneyChanged();
  void slotLightAt(QPoint p);

private:
  void fitInView() const;

private:
  QGraphicsView* v;
  TableScene* t;
  QLabel* m_l0;
  QLabel* m_l1;
  UserInput m_ui;
};

#endif // MAINWINDOW_H
