#ifndef TRAFFICSCENE_H
#define TRAFFICSCENE_H

#include "earth.h"
#include "transformscene.h"
#include <QObject>
#include <random>
#include <array>
#include <vector>

class QGraphicsTextItem;

class TrafficScene : public TransformScene
{
  Q_OBJECT

public:
  TrafficScene();
  virtual ~TrafficScene();
  double getGridSize() const { return ms_gridSize; }
  int getMoney(unsigned int player) const;
  QColor getPlayerColor(unsigned int player) const;

protected:
  void eventClick(QPointF p, PointerEvent::Color c) override;

private slots:
  void slotGo();
  void slotNext();

private:

  virtual void init();

private:
  struct Dot
  {
    unsigned int x,y;
    QGraphicsEllipseItem* ellipse;
    QBrush originalBrush;
  };
  struct Track
  {
    unsigned int from, to, player;
    QGraphicsLineItem* line;
  };
  struct PathSegment
  {
    unsigned int from, to;
    const Track* t;
  };

  void mutateMoney( unsigned int player, double amount );
  double getCost( unsigned int from, unsigned int to, std::vector<const Track*> path ) const;
  double dist( unsigned int from, unsigned int to ) const;
  double travelDist( unsigned int from, unsigned int to ) const;
  std::vector<const Track*> getBestPath(unsigned int from, unsigned int to) const;
  QGraphicsLineItem* addDotLine(int x1, int y1, int x2, int y2 , const QColor& colorC = QColor());
  double getBorder() const;
  QRectF getField() const;
  QPointF getDotOrigin() const;
  QPointF getDotDistance() const;
  double getMaxDotDistance() const;
  QPointF getDotPosition(double x, double y) const;
  void createCities();
  void addTrack(unsigned int from, unsigned int to);
  unsigned int getDot(unsigned int x, unsigned int y) const;

private:
  enum class TravelState { neutral, select1, select2, startTravel, travelling };
  enum class ClickState { neutral, click1 };

  static const unsigned int ms_gridSize = 10;
  std::mt19937 m_rng;
  Earth m_earth;
  std::vector<Dot> m_dots;
  std::vector<unsigned int> m_cities;
  TravelState m_travelState;
  ClickState m_clickState;
  unsigned int m_travel1;
  unsigned int m_travel2;
  unsigned int m_click1;
  std::vector<Track> m_tracks;
  std::vector<QGraphicsLineItem*> m_travelLines;
  std::array<double, 2> m_money;
  std::array<QGraphicsTextItem*, 2> m_moneyLabel;
  unsigned int m_currentPlayer = 0;
};

#endif // TRAFFICSCENE_H
