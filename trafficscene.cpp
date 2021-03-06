#include "mouseping.h"
#include "trafficscene.h"
#include "utilities.h"
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <QDebug>
#include <QTimer>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>

namespace
{
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
    m_money{ 35, 40 },
    m_moneyLabel{ 0, 0 }
{  
}

TrafficScene::~TrafficScene()
{  
}

void TrafficScene::createCities()
{
  std::uniform_int_distribution<unsigned int> range(0, m_dots.size());
  std::array<unsigned int, ms_gridSize> xCities;
  std::array<unsigned int, ms_gridSize> yCities;
  do
  {
    m_cities.clear();
    xCities.fill(0);
    yCities.fill(0);
    for (unsigned int i = 0; i != 12; ++i )
    {
      unsigned int n;
      do
      {
        n = range(m_rng);
      }
      while ( Utilities::contains( m_cities, n ) );

      m_cities.push_back(n);
      ++xCities[m_dots[n].x];
      ++yCities[m_dots[n].y];
    }
  }
  while ( xCities[0] == 0 || xCities[ms_gridSize-1] == 0 ||
          yCities[0] == 0 || yCities[ms_gridSize-1] == 0 );
}

void TrafficScene::init()
{
  static std::random_device rd;
  m_rng.seed(rd());

  double dotSize = getMaxDotDistance()/6+1;

  for ( unsigned int x = 0; x != ms_gridSize; ++x )
  {
    for ( unsigned int y = 0; y != ms_gridSize; ++y )
    {
      auto ellipse = addEllipse( Utilities::squareAt( getDotPosition(x,y), dotSize ),
                                 QPen(Qt::NoPen),
                                 QBrush(Qt::white, Qt::SolidPattern) );
      m_dots.push_back(Dot{x, y, ellipse, QBrush()});
    }
  }

  createCities();
  for (unsigned int i = 0; i != 12; ++i )
  {
    Dot dot = m_dots[m_cities[i]];
    QBrush brush = dot.ellipse->brush();
    brush.setColor(QColor(Qt::magenta));
    dot.ellipse->setBrush(brush);
    QFont f;
    f.setPixelSize(getMaxDotDistance()*0.4);
    QGraphicsTextItem* text = addText(QString::number(i+1), f);
    text->setPos(getDotPosition(dot.x,dot.y));
    text->setDefaultTextColor(Qt::white);
  }

  for ( auto&& d : m_dots )
  {
    d.originalBrush = d.ellipse->brush();
  }

  double heightMapDotSize = 0.3*getMaxDotDistance();
  for ( double x = -0.5; x <= ms_gridSize-0.5; x += 0.2 )
  {
    for ( double y = -0.5; y <= ms_gridSize-0.5; y += 0.2 )
    {
      QColor c;
      c.setHsv(m_earth.height(x, y), 255, 150);
      addEllipse(Utilities::squareAt(getDotPosition(x, y), heightMapDotSize), QPen(Qt::NoPen), QBrush(c, Qt::SolidPattern))->setZValue(-10);
    }
  }

  QFont font;
  font.setPixelSize(getBorder()*0.8);
  QRectF field = getField();
  for ( unsigned int player = 0; player != 2; ++player )
  {
    QGraphicsTextItem* text = addText("00", font);
    text->setDefaultTextColor(Qt::black);    
    QRectF brect = text->boundingRect();
    QPointF offset = brect.center()-brect.topLeft();
    text->setPos(QPointF( getTableRect().left() + 0.3 * field.width(),
                          getTableRect().top() + 0.5 * getBorder() + (getTableRect().height()-getBorder())*(!player) )
                 - offset );
    text->setTransformOriginPoint(offset);
    m_moneyLabel[player] = text;
    mutateMoney(player, 0);
  }
  m_moneyLabel[1]->setRotation(180);

//  addRect(getField(), QPen(Qt::red));


}

int TrafficScene::getMoney(unsigned int player) const
{
  return m_money[player];
}

QColor TrafficScene::getPlayerColor(unsigned int player) const
{
  switch (player)
  {
    case 0: return Qt::yellow;
    case 1: return Qt::cyan;
  }
  return Qt::black;
}

void TrafficScene::addTrack(unsigned int from, unsigned int to)
{
  double cost = 10*dist(from, to);
  if ( from != to && cost <= m_money[m_currentPlayer] )
  {
    mutateMoney(m_currentPlayer, -cost);
    int x1 = m_dots[from].x;
    int y1 = m_dots[from].y;
    int x2 = m_dots[to].x;
    int y2 = m_dots[to].y;
    std::vector<unsigned int> route;
    route.push_back(getDot(x1, y1));
    if ( x1 == x2 ) // vertical
    {
      int dy = Utilities::sign(y2-y1);
      for ( int y = y1+dy; y != y2; y += dy )
      {
        route.push_back(getDot(x1, y));
      }
    }
    else // not vertical
    {
      int dx = Utilities::sign(x2-x1);
      for ( int x = x1 + dx; x != x2; x += dx )
      {
        if ( ((x-x1) * (y2-y1)) % (x2-x1) == 0 )
        {
          int y = y1 + (x-x1) * (y2-y1)/(x2-x1);
          route.push_back(getDot(x, y));
        }
      }
    }
    route.push_back(getDot(x2, y2));
    for ( auto it = route.begin(); it != route.end() - 1; ++it)
    {
      QGraphicsLineItem* line = addDotLine(m_dots[*it].x, m_dots[*it].y,
                                           m_dots[*(it+1)].x, m_dots[*(it+1)].y,
                                           getPlayerColor(m_currentPlayer));
      m_tracks.push_back(Track{*it, *(it+1), m_currentPlayer, line});
    }
    for ( unsigned int d : route )
    {
      addItem( new MousePing(getDotPosition(m_dots[d].x,m_dots[d].y), QColor(Qt::black) ) );
    }
  }
}

unsigned int TrafficScene::getDot(unsigned int x, unsigned int y) const
{
  for ( unsigned int i = 0; i != m_dots.size(); ++i )
  {
    if ( m_dots[i].x == x &&
         m_dots[i].y == y )
    {
      return i;
    }
  }
  throw std::logic_error("dot not found");
}

void TrafficScene::eventClick(QPointF p, PointerEvent::Color /*c*/)
{
  auto its = items(p);
  for ( auto l : m_moneyLabel )
  if ( its.contains(l) )
  {
    slotGo();
  }
  if ( m_travelState == TravelState::neutral )
  {
    unsigned int to = 0;
    for ( ; to != m_dots.size(); ++to )
    {
      if ( its.contains(  m_dots[to].ellipse ) )
      {
        break;
      }
    }
    if ( to != m_dots.size() )
    {
      switch( m_clickState )
      {
        case ClickState::neutral:
          m_clickState = ClickState::click1;
          m_click1 = to;
          m_dots[m_click1].ellipse->setBrush(getPlayerColor(m_currentPlayer));
        break;
        case ClickState::click1:
        {
          m_clickState = ClickState::neutral;
          m_dots[m_click1].ellipse->setBrush(m_dots[m_click1].originalBrush);
          addTrack(m_click1, to);
        }
        break;
      }
    }

  }
}

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
    {
      m_travelState = TravelState::select2;
      m_travel1 = static_cast<unsigned int>(sqrt(cityRange(m_rng)));
      Dot dot = m_dots[m_cities[m_travel1]];
      dot.ellipse->setBrush(QColor(Qt::blue));
      addItem( new MousePing(getDotPosition(dot.x,dot.y), QColor(Qt::blue) ) );
    }
    break;
    case TravelState::select2:
    {
      m_travelState = TravelState::startTravel;
      do
      {
        m_travel2 = static_cast<unsigned int>(sqrt(cityRange(m_rng)));
      }
      while (m_travel2 == m_travel1);
      Dot dot = m_dots[m_cities[m_travel2]];
      dot.ellipse->setBrush(QColor(Qt::blue));
      addItem( new MousePing(getDotPosition(dot.x,dot.y), QColor(Qt::blue) ) );
    }
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
        m_travelLines.push_back(addDotLine(dot1.x, dot1.y, dot2.x, dot2.y));
        m_travelLines.push_back(addDotLine(dot2.x, dot2.y, dot3.x, dot3.y));
        at = t2;
      }
      const Dot& dot1 = m_dots[at];
      const Dot& dot2 = m_dots[m_cities[m_travel2]];
      m_travelLines.push_back(addDotLine(dot1.x, dot1.y, dot2.x, dot2.y));
    }
    break;
    case TravelState::travelling:
      m_travelState = TravelState::neutral;
      m_dots[m_cities[m_travel1]].ellipse->setBrush(m_dots[m_cities[m_travel1]].originalBrush);
      m_dots[m_cities[m_travel2]].ellipse->setBrush(m_dots[m_cities[m_travel2]].originalBrush);
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
  m_moneyLabel[player]->setHtml("<div style='background-color: " +
                                getPlayerColor(player).name() +
                                ";'>" + QString::number(m_money[player]) + "</div>");
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

QGraphicsLineItem* TrafficScene::addDotLine(int x1, int y1, int x2, int y2, const QColor& color)
{
  QGraphicsLineItem* line = addLine(QLineF(getDotPosition(x1, y1), getDotPosition(x2, y2)));
  QPen pen;
  pen.setWidthF( 0.1 * getMaxDotDistance() );
  pen.setColor(color);
  line->setPen(pen);
  return line;
}

double TrafficScene::getBorder() const
{
  return 0.05 * getTableRect().height();
}

QRectF TrafficScene::getField() const
{
  QRectF rect = getTableRect();
  double space = getBorder();
  rect.setTop(rect.top() + space);
  rect.setBottom(rect.bottom() - space);
  return rect;
}

QPointF TrafficScene::getDotOrigin() const
{
  return getField().topLeft() + 0.5*getDotDistance();
}

QPointF TrafficScene::getDotDistance() const
{
  QRectF field = getField();
  return (field.bottomRight() - field.topLeft()) / ms_gridSize;
}

double TrafficScene::getMaxDotDistance() const
{
  QPointF dotDistance = getDotDistance();
  return std::max(dotDistance.x(), dotDistance.y());
}

QPointF TrafficScene::getDotPosition( double x, double y ) const
{
  QPointF dotDistance = getDotDistance();
  return getDotOrigin() + QPoint( dotDistance.x()*x,
                                  dotDistance.y()*y );

}
