#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QGraphicsView;
class TableScene;
class QLabel;

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

private:
  void fitInView() const;

private:
  QGraphicsView* v;
  TableScene* t;
  QLabel* m_l0;
  QLabel* m_l1;
};

#endif // MAINWINDOW_H
