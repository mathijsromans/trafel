#include "mainwindow.h"
#include "tablescene.h"
#include "transformscene.h"
#include "userinput.h"
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
  t = new TableScene();
  v->setScene(t);
//  QPushButton* b = new QPushButton("Go!");
  m_l0 = new QLabel("Player 1");
  m_l1 = new QLabel("Player 2");

//  QPalette pal = m_l0->palette();

//  pal.setColor( m_l0->backgroundRole(), t->getColor(0));
//  m_l0->setAutoFillBackground(true);
//  m_l0->setPalette(pal);

//  pal.setColor( m_l1->backgroundRole(), t->getColor(1));
//  m_l1->setAutoFillBackground(true);
//  m_l1->setPalette(pal);

//  QVBoxLayout* buttonLayout = new QVBoxLayout();
//  buttonLayout->addWidget(m_l0);
//  buttonLayout->addWidget(b);
//  buttonLayout->addWidget(m_l1);
//  QWidget* buttons = new QWidget();
//  buttons->setLayout(buttonLayout);
  QHBoxLayout* layout = new QHBoxLayout();
//  layout->addWidget(buttons);
  layout->addWidget(v);
  QWidget* w = new QWidget();
  w->setLayout( layout );
  setCentralWidget(w);

//  connect( b, SIGNAL(pressed()), t, SLOT(slotGo()) );
  connect( t, SIGNAL(signalMoneyChanged()), this, SLOT(slotMoneyChanged()) );

  UserInput* ui = new UserInput();
  connect(ui, SIGNAL(signalMouseClick(PointerEvent)), t, SLOT(slotLightAt(PointerEvent)));

  t->calibrate();
  showMaximized();
  slotMoneyChanged();

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

void MainWindow::slotMoneyChanged()
{
  m_l0->setText(QString::number(t->getMoney(0)));
  m_l1->setText(QString::number(t->getMoney(1)));
}

void MainWindow::slotLightAt(PointerEvent e)
{
  qDebug() << e.getAny();
}

void MainWindow::fitInView() const
{
  v->setSceneRect(QRectF(0,0,1000,800));
  qDebug() << "sceneRect " << v->sceneRect();
//  v->fitInView(t->itemsBoundingRect(), Qt::KeepAspectRatio);
}
