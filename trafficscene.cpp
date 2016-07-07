#include "trafficscene.h"
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <QDebug>
#include <QTimer>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

namespace
{
template <typename T1, typename T2>
bool contains( const T1& vec, const T2& elem )
{
  return std::find( vec.begin(), vec.end(), elem ) != vec.end();
}

template <typename T1, typename T2>
bool has_if( const T1& vec, T2 condition )
{
  return std::find_if( vec.begin(), vec.end(), condition ) != vec.end();
}

template <typename T>
T sqr(T x) { return x*x; }

}


TrafficScene::TrafficScene()
  : m_earth(ms_gridSize, ms_gridSize),
    m_travelState(TravelState::neutral),
    m_clickState(ClickState::neutral),
    m_money{ 35, 45 }
{  
}

TrafficScene::~TrafficScene()
{  
}

void TrafficScene::init()
{
  std::random_device rd;
  m_rng.seed(rd());

  for ( unsigned int x = 0; x != ms_gridSize; ++x )
  {
    for ( unsigned int y = 0; y != ms_gridSize; ++y )
    {
      m_dots.push_back(Dot{x, y, addEllipse( squareAt(10*x, 10*y, 1), QPen(Qt::white)), QPen()});
    }
  }

  std::uniform_int_distribution<unsigned int> range(0, m_dots.size());
  for (unsigned int i = 0; i != 12; ++i )
  {
    unsigned int n;
    do
    {
      n = range(m_rng);
    }
    while ( contains( m_cities, n ) );

    m_cities.push_back(n);
    m_dots[n].ellipse->setPen(QColor(Qt::magenta));
    QFont f;
    f.setPixelSize(5);
    QGraphicsTextItem* text = addText(QString::number(i+1), f);
    text->setPos(10.0*m_dots[n].x-4, 10.0*m_dots[n].y-4);
    text->setDefaultTextColor(Qt::white);
  }

  for ( auto&& d : m_dots )
  {
    d.originalPen = d.ellipse->pen();
  }

  for ( int x = -5; x != 10*ms_gridSize-5; ++x )
  {
    for ( int y = -5; y != 10*ms_gridSize-5; ++y )
    {
      QColor c;
      c.setHsv(m_earth.height(0.1*x, 0.1*y), 255, 150);
      addEllipse(squareAt(x, y, 0.93), QPen(c))->setZValue(-10);
    }
  }
}

int TrafficScene::getMoney(unsigned int player) const
{
  return m_money[player];
}

QColor TrafficScene::getColor(unsigned int player) const
{
  switch (player)
  {
    case 0: return Qt::yellow;
    case 1: return Qt::cyan;
  }
  return Qt::black;
}

//void TrafficScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
//{
//  if ( m_travelState == TravelState::neutral )
//  {
//    auto its = items(event->scenePos());
//    unsigned int i = 0;
//    for ( ; i != m_dots.size(); ++i )
//    {
//      if ( its.contains(  m_dots[i].ellipse ) )
//      {
//        break;
//      }
//    }
//    if ( i != m_dots.size() )
//    {
//      switch( m_clickState )
//      {
//        case ClickState::neutral:
//          m_clickState = ClickState::click1;
//          m_click1 = i;
//          m_dots[m_click1].ellipse->setPen(getColor(m_currentPlayer));
//        break;
//        case ClickState::click1:
//        {
//          m_clickState = ClickState::neutral;
//          m_dots[m_click1].ellipse->setPen(m_dots[m_click1].originalPen);
//          double cost = 10*dist(m_click1, i);
//          if ( m_click1 != i && cost <= m_money[m_currentPlayer] )
//          {
//            mutateMoney(m_currentPlayer, -cost);
//            m_tracks.push_back(Track{m_click1, i, m_currentPlayer,
//                             addLine(10*m_dots[m_click1].x,
//                                     10*m_dots[m_click1].y,
//                                     10*m_dots[i].x,
//                                     10*m_dots[i].y, QPen(getColor(m_currentPlayer)) ) });
//          }
//        }
//        break;
//      }
//    }

//  }
//}

void TrafficScene::slotGo()
{
  if ( m_travelState == TravelState::neutral &&
       m_clickState == ClickState::neutral )
  {
    slotNext();
  }
}

void TrafficScene::slotNext()
{
  std::uniform_int_distribution<unsigned int> cityRange(0, sqr(m_cities.size())-1);

  switch( m_travelState )
  {
    case TravelState::select1:
      m_travelState = TravelState::select2;
      m_travel1 = static_cast<unsigned int>(sqrt(cityRange(m_rng)));
      m_dots[m_cities[m_travel1]].ellipse->setPen(QColor(Qt::blue));
    break;
    case TravelState::select2:
      m_travelState = TravelState::startTravel;
      do
      {
        m_travel2 = static_cast<unsigned int>(sqrt(cityRange(m_rng)));
      }
      while (m_travel2 == m_travel1);
      m_dots[m_cities[m_travel2]].ellipse->setPen(QColor(Qt::blue));
    break;
    case TravelState::startTravel:
    {
      m_travelState = TravelState::travelling;
      const std::vector<const Track*> bestPath = getBestPath(m_cities[m_travel1], m_cities[m_travel2]);
      unsigned int at = m_cities[m_travel1];
      for ( const Track* t: bestPath )
      {
        mutateMoney(t->player, 5*dist(t->from, t->to));
        unsigned int t1 = t->from;
        unsigned int t2 = t->to;
        double d1 = dist(at, t1);
        double d2 = dist(at, t2);
        if ( d2 < d1 )
        {
          std::swap(t1, t2);
        }
        const Dot& dot1 = m_dots[at];
        const Dot& dot2 = m_dots[t1];
        const Dot& dot3 = m_dots[t2];
        m_travelLines.push_back(addLine(10*dot1.x, 10*dot1.y, 10*dot2.x, 10*dot2.y));
        m_travelLines.push_back(addLine(10*dot2.x, 10*dot2.y, 10*dot3.x, 10*dot3.y));
        at = t2;
      }
      const Dot& dot1 = m_dots[at];
      const Dot& dot2 = m_dots[m_cities[m_travel2]];
      m_travelLines.push_back(addLine(10*dot1.x, 10*dot1.y, 10*dot2.x, 10*dot2.y));
    }
    break;
    case TravelState::travelling:
      m_travelState = TravelState::neutral;
      m_dots[m_cities[m_travel1]].ellipse->setPen(m_dots[m_cities[m_travel1]].originalPen);
      m_dots[m_cities[m_travel2]].ellipse->setPen(m_dots[m_cities[m_travel2]].originalPen);
      for ( auto l : m_travelLines )
      {
        delete l;
      }
      m_travelLines.clear();
      m_currentPlayer = (m_currentPlayer+1)%2;
    break;
    case TravelState::neutral:
      m_travelState = TravelState::select1;
    break;
  }
  if ( m_travelState != TravelState::neutral )
  {
    QTimer::singleShot(1000, this, SLOT(slotNext()));
  }

}

void TrafficScene::mutateMoney(unsigned int player, double amount)
{
  m_money[player] += amount;
  signalMoneyChanged();
}

double TrafficScene::getCost(unsigned int from, unsigned int to, std::vector<const TrafficScene::Track*> path) const
{
  unsigned int at = from;
  double cost = 0;
  for ( const Track* t : path )
  {
    unsigned int t1 = t->from;
    unsigned int t2 = t->to;
    double d1 = dist(at, t1);
    double d2 = dist(at, t2);
    if ( d2 < d1 )
    {
      std::swap(t1, t2);
      std::swap(d1, d2);
    }
    if ( at != t1 )
    {
      cost += 1e6*d1;
    }
    cost += dist(t1, t2);
    at = t2;
  }
  if ( at != to )
  {
    cost += 1e6*dist(at, to);
  }
  return cost;
}

double TrafficScene::dist(unsigned int from, unsigned int to) const
{
  return sqrt(sqr(m_dots[from].x-m_dots[to].x)+sqr(m_dots[from].y-m_dots[to].y));
}

double
TrafficScene::travelDist(unsigned int from, unsigned int to) const
{
  double d = dist(from, to);
  for ( const Track& t : m_tracks )
  {
    if ( ( from == t.from && to == t.to ) ||
         ( from == t.to && to == t.from ) )
    {
      return d;
    }
  }
  return 1e6*d;
}

std::vector<const TrafficScene::Track*> TrafficScene::getBestPath(unsigned int from, unsigned int to) const
{
  std::set<unsigned int> Q;
  Q.insert(from);
  Q.insert(to);
  for ( const Track& t : m_tracks )
  {
    Q.insert(t.from);
    Q.insert(t.to);
  }
  const std::vector<unsigned int> all(Q.begin(), Q.end());

  std::map<unsigned int, double> dist;
  std::map<unsigned int, unsigned int> prev;

  for ( unsigned int node : Q )
  {
    dist[node] = std::numeric_limits<double>::max();
    prev[node] = 0;
  }

  dist[from] = 0;

  while (!Q.empty())
  {

    // u ← vertex in Q with min dist[u]    // Source node will be selected first
    double minDist = std::numeric_limits<double>::max();
    unsigned int u = -1;
    for ( unsigned int node : Q )
    {
      if ( dist[node] < minDist )
      {
        minDist = dist[node];
        u = node;
      }
    }
    assert( u != static_cast<unsigned int>(-1) );

    Q.erase(u);
    for ( unsigned int v : all )
    {
      if ( v != u )
      {
        double alt = dist[u] + travelDist(u, v);
        if ( alt < dist[v] )
        {
          dist[v] = alt;
          prev[v] = u;
        }
      }
    }
  }

  std::vector<const Track*> result;
  unsigned int at = to;
  while (at != from)
  {
    unsigned int p = prev[at];
    for ( const Track& t : m_tracks )
    {
      if ( ( p == t.from && at == t.to ) ||
           ( p == t.to && at == t.from ) )
      {
        result.push_back(&t);
        break;
      }
    }
    at = p;
  }
  std::reverse(result.begin(), result.end());
  return result;


//  5      for each vertex v in Graph:             // Initialization
//  6          dist[v] ← INFINITY                  // Unknown distance from source to v
//  7          prev[v] ← UNDEFINED                 // Previous node in optimal path from source
//  8          add v to Q                          // All nodes initially in Q (unvisited nodes)
//  9
// 10      dist[source] ← 0                        // Distance from source to source
// 11
// 12      while Q is not empty:
// 13          u ← vertex in Q with min dist[u]    // Source node will be selected first
// 14          remove u from Q
// 15
// 16          for each neighbor v of u:           // where v is still in Q.
// 17              alt ← dist[u] + length(u, v)
// 18              if alt < dist[v]:               // A shorter path to v has been found
// 19                  dist[v] ← alt
// 20                  prev[v] ← u
// 21
// 22      return dist[], prev[]

}

