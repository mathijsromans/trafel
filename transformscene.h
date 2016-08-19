#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include "pointerevent.h"
#include <QGraphicsScene>
#include <QPoint>
#include <memory>
#include <vector>

class Button;
class QGraphicsPixmapItem;
class QGraphicsTextItem;

class TransformScene : public QGraphicsScene
{
  Q_OBJECT

public:
  explicit TransformScene();
  virtual ~TransformScene();
  void calibrate();
  void inputEvent(const PointerEvent& e);
  void doInit(QRectF tableRect);
  void showInfoText(const std::string& text);
  QRectF getTableRect() const;

  /// returns unit vector with origin at the player's position
  QLineF getPlayerPosition(unsigned int player);
  unsigned int getNumPlayers() const;

signals:
  void signalMouseEvent(PointerEvent e);
  void signalQuit();

public slots:
  void slotQuit();
  void slotQuitYes();
  void slotQuitNo();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void processMouseEvent(PointerEvent e);
  void setScore(unsigned int player, int score);
  virtual double getFps() const { return 0; }
private:
  virtual void init() = 0;
  virtual void step(unsigned int /*turn*/) {}
  virtual void eventClick(QPointF /*p*/, PointerEvent::Color /*c*/) {}
  virtual void eventUnclick(QPointF /*p*/, PointerEvent::Color /*c*/) {}
  virtual void eventMove(QPointF /*p*/, PointerEvent::Color /*c*/) {}
  virtual bool showScore() const { return false; }

private slots:
  void doStep();

private:
  struct PlayerScore
  {
    QGraphicsTextItem* scoreText;
  };

  QRectF m_tableRect;
  QGraphicsTextItem* m_infoText;
  Button* m_quitYes;
  Button* m_quitNo;
  std::vector<PlayerScore> m_players;
  std::array<QPointF, 3> m_inputPrev;
  PointerEvent m_lastMouseEvent;
  unsigned int m_turn = 0;
  QTimer* m_timer;
};

#endif // TRANSFORMSCENE_H
