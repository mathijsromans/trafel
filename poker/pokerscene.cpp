#include "pokerscene.h"

#include <QGraphicsPixmapItem>
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <random>
#include <QTimer>
#include <stdlib.h>
#include <QDebug>

PokerScene::PokerScene()
 : TransformScene(),
   m_cards{{}},
   m_cardsBack{{}},
   m_numberCardsUsed(0),
   m_stage{Stage::start}
{
}

void PokerScene::init()
{
  if ( getNumPlayers() > 23 )
  {
    throw std::runtime_error("Too many players");
  }
  createCards();
}

void PokerScene::eventClick(QPointF /*p*/, PointerEvent::Color /*c*/)
{
  QRectF brect = m_cards.front()->boundingRect();
  QPointF cardOffset = brect.center()-brect.topLeft();
  switch (m_stage)
  {
   case Stage::start:
      shuffleCards();
      dealCards();
      m_stage = Stage::dealing;
      break;
    case Stage::dealing:
      break;
    case Stage::dealt:
    {
      for ( int i = 0; i != 3; ++i )
      {
        Card* card = m_cards[m_numberCardsUsed];
        card->setPos(getTableRect().center() + QPoint(-i * 1.05 * brect.width(), 0) - cardOffset );
        card->show();
        m_numberCardsUsed++;
      }
      playDealCardSound();
      m_stage = Stage::flop;
    }
      break;
    case Stage::flop:
    {
      m_stage = Stage::turn;
      Card* card = m_cards[m_numberCardsUsed];
      card->setPos(getTableRect().center() + QPoint(+1 * 1.05 * brect.width(), 0) - cardOffset );
      card->show();
      playDealCardSound();
      m_numberCardsUsed++;
    }
      break;
    case Stage::turn:
    {
      m_stage = Stage::river;
      Card* card = m_cards[m_numberCardsUsed];
      card->setPos(getTableRect().center() + QPoint(+2 * 1.05 * brect.width(), 0) - cardOffset );
      card->show();
      playDealCardSound();
      m_numberCardsUsed++;
    }
      break;
    case Stage::river:
      hideCards();
      m_stage = Stage::start;
      break;
  }
}

void PokerScene::hideCards()
{
  for ( Card* card : m_cards )
  {
    card->hide();
    card->setRotation(0);
  }
  for ( Card* card : m_cardsBack )
  {
    card->hide();
    card->setRotation(0);
  }
}

void PokerScene::createCards()
{
  const double cardRatio = 1.2/0.8;
  double cardWidth = 0.1 * getTableRect().width();
  QRectF cardSize( 0, 0, cardWidth, cardRatio * cardWidth );
  QPixmap cardsFrontPixMap = QPixmap("../trafel/resources/cardsfront.png");
  QPixmap cardsBackPixMap = QPixmap("../trafel/resources/cardback.png").scaled(cardWidth, cardRatio * cardWidth);

  for (unsigned int suit = 0; suit < numSuit; ++suit )
  {
    for ( unsigned int value = 0; value < numValue; ++value )
    {
      static const QRect readSize( 0, 0, 225, 340 );
      QRect cut = readSize.translated( value * readSize.width(), suit * readSize.height() );
      QPixmap one = cardsFrontPixMap.copy( cut );
      Card *item = new Card(one.scaled(cardWidth, cardSize.height()));
      item->setFlag( QGraphicsItem::ItemIsMovable );
      addItem(item);
      item->hide();
      item->setTransformOriginPoint(cardSize.bottomRight() / 2);
      m_cards[suit * numValue + value] = item;
      Card *back = new Card(cardsBackPixMap);
      addItem(back);
      back->hide();
      back->setTransformOriginPoint(cardSize.bottomRight() / 2);
      m_cardsBack[suit * numValue + value] = back;
    }
  }
}

void PokerScene::shuffleCards()
{
  static std::random_device rd;
  static std::mt19937 rng(rd());
  std::shuffle( m_cards.begin(), m_cards.end(), rng );
}

void PokerScene::dealCards()
{
  const unsigned int cardsDealt = 2;
  m_numberCardsUsed = 0;
  for ( unsigned int i = 0; i < getNumPlayers() * cardsDealt; ++i )
  {
    QTimer::singleShot(300*i, this, SLOT(dealCard()));
  }
  QTimer::singleShot(300*getNumPlayers() * cardsDealt + 50, this, SLOT(showCards()));
}

void PokerScene::playDealCardSound()
{
  system("omxplayer -o local ../poker/sound/dealcard.wav&");
}

void PokerScene::dealCard()
{
  const QRectF tableRect = getTableRect();
  Card* card = m_cardsBack[m_numberCardsUsed];
  QRectF brect = card->boundingRect();
  QPointF cardOffset = brect.center()-brect.topLeft();
  const double moveDistanceX = 0.5 * tableRect.width()  - cardOffset.y();
  const double moveDistanceY = 0.5 * tableRect.height() - cardOffset.y();
  QTimeLine *timer = new QTimeLine(300, this);
  timer->setFrameRange(0, 100);
  double adjacentCardAngleOffset = 0.6 * brect.width();
  QGraphicsItemAnimation *animation = new QGraphicsItemAnimation(this);
  animation->setTimeLine(timer);
  int dealRound = m_numberCardsUsed / getNumPlayers();
  double offset = adjacentCardAngleOffset * ( 1 - 2 * dealRound );

  unsigned int position = m_numberCardsUsed + dealRound + 1;
  double angle = 2 * M_PI * position / (getNumPlayers() + 1);
  for (double d = 0; d < 1; d = d + 0.01)
  {
    animation->setPosAt(d, tableRect.center() +
                           d * QPointF(-cos(angle) * moveDistanceX + offset*sin(angle), sin(angle)* moveDistanceY+offset*cos(angle))
                           - cardOffset );
  }
  animation->setItem(card);
  card->setPos(tableRect.center());
  card->show();
  m_cardsBack[m_numberCardsUsed]->setRotation(-180 * angle / M_PI + 90);
  timer->start();
  playDealCardSound();
  ++m_numberCardsUsed;
}

void PokerScene::showCards()
{
  for ( unsigned int i = 0; i < m_numberCardsUsed; ++i )
  {
    m_cards[i]->setPos(m_cardsBack[i]->pos());
    m_cards[i]->setRotation(m_cardsBack[i]->rotation());
    m_cardsBack[i]->hide();
    m_cards[i]->show();
  }
  m_stage = Stage::dealt;
}
