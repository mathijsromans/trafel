#ifndef TABLESCENE_H
#define TABLESCENE_H

#include "earth.h"
#include "transformscene.h"
#include <QObject>
#include <random>
#include <array>
#include <vector>

class TableScene : public TransformScene
{
  Q_OBJECT

public:
  TableScene();
  virtual ~TableScene();
  double getGridSize() const { return ms_gridSize; }
  int getMoney(unsigned int player) const;
  QColor getColor(unsigned int player) const;

signals:
  void signalMoneyChanged();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event);

private slots:
  void slotGo();
  void slotNext();

private:
  struct Dot
  {
    unsigned int x,y;
    QGraphicsEllipseItem* ellipse;
    QPen originalPen;
  };
  struct Track
  {
    unsigned int from, to, player;
    QGraphicsLineItem* ellipse;
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
  unsigned int m_currentPlayer = 0;


};

#endif // TABLESCENE_H
