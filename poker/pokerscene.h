#ifndef TABLESCENE_H
#define TABLESCENE_H

#include "transformscene.h"

class PokerScene : public TransformScene
{
  Q_OBJECT
public:
  explicit PokerScene( unsigned int numPlayers );
  virtual ~PokerScene() {}
private:
  enum class Stage { start, dealing, dealt, flop, turn, river };
  void hideCards();
  void shuffleCards();
  void createCards();
  void dealCards();
  void playDealCardSound();
private slots:
  void dealCard();
  void showCards();
private:
  void init() override;
  void mouseClick(QPointF p) override;
private:
  typedef QGraphicsPixmapItem Card;
  static const unsigned int numValue = 13;
  static const unsigned int numSuit = 4;
  std::array<Card*, numValue*numSuit> m_cards;
  std::array<Card*, numValue*numSuit> m_cardsBack;
  unsigned int m_numPlayers;
  unsigned int m_numberCardsUsed;
  Stage m_stage;
};

#endif // TABLESCENE_H
