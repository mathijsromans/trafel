#include "button.h"
#include "calibration.h"
#include "mouseping.h"
#include "transformscene.h"
#include "utilities.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QPainter>
#include <QTimer>

#include <iostream>

TransformScene::TransformScene()
 : m_infoText(0),
   m_quitYes(new Button("Yes")),
   m_quitNo(new Button("No")),
   m_inputPrev{{}},
   m_turn(0),
   m_timer(0)
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

void TransformScene::inputEvent(const PointerEvent& e)
{
  for ( PointerEvent::CPoint p : e.getPoints() )
  {
    foreach ( QGraphicsItem* item, items(p.point))
    {
      ClickableItem* clickItem = dynamic_cast<ClickableItem*>( item );
      if ( clickItem )
      {
        clickItem->eventClick(p);
        return;
      }
    }
  }

  for ( unsigned int c = 0; c != 3; ++c )
  {
    PointerEvent::Color color = PointerEvent::Color(c);
    QPointF prevPoint = m_inputPrev[c];
    QPointF newPoint = e.getPoint(color);
    if ( prevPoint.isNull() && !newPoint.isNull() )
    {
      eventClick( newPoint, color );
    }
    else if ( !prevPoint.isNull() && newPoint.isNull() )
    {
      eventUnclick( newPoint, color );
    }
    else if ( !prevPoint.isNull() && !newPoint.isNull() &&
              Utilities::dist(prevPoint, newPoint) > 6 )
    {
      eventMove( newPoint, color );
    }
    m_inputPrev[c] = newPoint;
  }
}

void TransformScene::doInit(QRectF tableRect)
{
  m_tableRect = tableRect;
  Button* quitButton = new Button("Quit");
  quitButton->setPos(getTableRect().topLeft());
  addItem( quitButton );
  connect(quitButton, SIGNAL(pressed()), this, SLOT(slotQuit()));
//  addRect(getTableRect(), QPen(Qt::white));

  init();

  if ( showScore() )
  {
    QFont font;
    double size = getTableRect().height();
    font.setPixelSize(0.04 * size);
    for ( unsigned int player = 0; player != getNumPlayers(); ++player )
    {
      QGraphicsTextItem* text = addText("00", font);
      text->setDefaultTextColor(Qt::black);
      QRectF brect = text->boundingRect();
      QPointF offset = brect.center()-brect.topLeft();
      QLineF playerLine = getPlayerPosition(player);
      playerLine.setLength(0.5*brect.height());
      text->setPos(playerLine.p2() - 0.1 * size * playerLine.normalVector().unitVector().translated(-playerLine.p1()).p2() - offset );
      text->setTransformOriginPoint(offset);
      text->setRotation(90-playerLine.angle());
      m_players.push_back( PlayerScore{text} );
      setScore(player, 0);
    }
  }


  double fps = getFps();
  if ( fps != 0 )
  {
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(doStep()));
    m_timer->start(1000/fps);
    step(0);
  }
}

void TransformScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  processMouseEvent(PointerEvent(std::array<QPointF, 3>{event->scenePos(), QPointF(0,0), QPointF(0,0)}));
}

void TransformScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* /*event*/)
{
  processMouseEvent(PointerEvent(std::array<QPointF, 3>{QPointF(0,0), QPointF(0,0), QPointF(0,0)}));
}

void TransformScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if ( event->buttons() == Qt::LeftButton )
  {
    processMouseEvent(PointerEvent(std::array<QPointF, 3>{event->scenePos(), QPointF(0,0), QPointF(0,0)}));
  }
}

void TransformScene::processMouseEvent(PointerEvent e)
{
  if ( e.compareTo(m_lastMouseEvent) )
  {
    emit signalMouseEvent(e);
    m_lastMouseEvent = e;
  }
}

void TransformScene::slotQuit()
{
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
  emit signalQuit();
}

void TransformScene::slotQuitNo()
{
  showInfoText("");
  m_quitYes->hide();
  m_quitNo->hide();
}

unsigned int TransformScene::getNumPlayers() const
{
  return m_numPlayers;
}

QRectF TransformScene::getTableRect() const
{
  return m_tableRect;
}

QLineF TransformScene::getPlayerPosition(unsigned int player)
{
  player = player % getNumPlayers();
  QRectF tableRect = getTableRect();
  double angle = 2 * M_PI * (player+1) / (getNumPlayers() + 1);
  QPointF origin = tableRect.center() + QPointF(-0.5 * tableRect.width()  * cos(angle),
                                                 0.5 * tableRect.height() * sin(angle) );
  QLineF line( origin, tableRect.center() );
  return line.unitVector();
}

void TransformScene::setScore(unsigned int player, int score)
{
  if ( player < m_players.size() )
  {
    const std::array<QColor, 3> colors{QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue)};
    m_players[player].scoreText->setHtml("<div style='background-color: " +
                                    colors[player%colors.size()].name() +
                                    ";'>" + QString::number(score) + "</div>");
  }
}

void TransformScene::setNumPlayers(int num)
{
  m_numPlayers = num;
}

void TransformScene::doStep()
{
  step(m_turn++);
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
