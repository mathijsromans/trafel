#include "button.h"
#include "mouseping.h"
#include "transformscene.h"
#include "utilities.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QPainter>

#include <iostream>

TransformScene::TransformScene()
 : m_calibration(*this),
   m_infoText(0),
   m_quitYes(new Button("Yes")),
   m_quitNo(new Button("No"))
{
  setBackgroundBrush(Qt::black);
  addItem(m_quitYes);
  addItem(m_quitNo);
  m_quitYes->hide();
  m_quitNo->hide();
  connect(m_quitYes, SIGNAL(pressed()), this, SLOT(slotQuitYes()));
  connect(m_quitNo, SIGNAL(pressed()), this, SLOT(slotQuitNo()));
}

TransformScene::~TransformScene()
{
}

void TransformScene::calibrate()
{
  m_calibration.calibrate();
}

void TransformScene::mouseClick(QPointF /*p*/)
{
}

void TransformScene::inputEvent(const PointerEvent& e)
{
  bool accepted = false;
  for ( PointerEvent::CPoint p : e.getPoints() )
  {
    foreach ( QGraphicsItem* item, items(p.point))
    {
      ClickableItem* clickItem = dynamic_cast<ClickableItem*>( item );
      if ( clickItem )
      {
        clickItem->mouseClick(p);
        accepted = true;
      }
    }
  }

  if ( !accepted )
  {
   mouseClick(e.getAny());
  }
}

void TransformScene::doInit()
{
  Button* quitButton = new Button("Quit");
  quitButton->setPos(getTableRect().topLeft());
  addItem( quitButton );
  connect(quitButton, SIGNAL(pressed()), this, SLOT(slotQuit()));
  addRect(getTableRect(), QPen(Qt::white));

  init();
}

void TransformScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  m_calibration.processMouseClick(PointerEvent(std::array<QPointF, 3>{event->scenePos(), QPointF(0,0), QPointF(0,0)}));
}

void TransformScene::slotLightAt(PointerEvent e)
{
  m_calibration.processMouseClick( e );
}

void TransformScene::slotQuit()
{
  qDebug() << "QUIT";
  showInfoText("Do you want to quit?");
  QRectF rect = getTableRect();
  m_quitYes->setPos( rect.center() + QPointF(0.25 * rect.width(), 0));
  m_quitNo->setPos( rect.center() + QPointF(-0.25 * rect.width(), 0));
  m_quitYes->show();
  m_quitNo->show();
}

void TransformScene::slotQuitYes()
{
  showInfoText("");
  m_quitYes->hide();
  m_quitNo->hide();
  quit();
}

void TransformScene::slotQuitNo()
{
  showInfoText("");
  m_quitYes->hide();
  m_quitNo->hide();
}


QRectF TransformScene::getTableRect() const
{
  return m_calibration.getTableRect();
}

QLineF TransformScene::getPlayerPosition(unsigned int player, unsigned int numPlayers)
{
  QRectF tableRect = getTableRect();
  double angle = 2 * M_PI * player / (numPlayers + 1);
  QPointF origin = tableRect.center() + QPointF(-0.5 * tableRect.width()  * cos(angle),
                                                 0.5 * tableRect.height() * sin(angle) );
  QLineF line( origin, tableRect.center() );
  return line.unitVector();
}

void TransformScene::showInfoText(const std::string& text)
{
  if ( !m_infoText )
  {
    m_infoText = addText("");
  }
  QFont font;
  font.setPixelSize(40);
  m_infoText->setFont(font);
  m_infoText->setDefaultTextColor(Qt::white);
  m_infoText->setPlainText(QString::fromStdString(text));
  m_infoText->setPos(200, 200);
  m_infoText->show();
}
