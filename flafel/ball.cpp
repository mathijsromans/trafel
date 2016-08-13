#include "ball.h"

#include "flafelscene.h"
#include "pixmapitem.h"
#include "stopper.h"
#include "utilities.h"

std::unique_ptr<QPixmap> Ball::ms_pixmap;
double Ball::ms_diameter = -1;

Ball::Ball(FlafelScene& scene)
{
  auto tableRect = scene.getTableRect();
  if ( !ms_pixmap )
  {
    if ( ms_diameter < 0 )
    {
      ms_diameter = 0.05 * scene.getTableRect().width();
    }
    ms_pixmap = std::make_unique<QPixmap>(QPixmap("../trafel/resources/metal_ball_C0_cut.png").scaledToWidth(ms_diameter) );
  }

  static std::random_device rd;
  static std::mt19937 rng(rd());
  std::uniform_real_distribution<double> angleRange(0, 2 * M_PI);
  std::uniform_real_distribution<double> xRange( tableRect.center().x() - 0.25 * tableRect.width(),
                                                 tableRect.center().x() + 0.25 * tableRect.width() );
  std::uniform_real_distribution<double> yRange( tableRect.center().y() - 0.25 * tableRect.height(),
                                                 tableRect.center().y() + 0.25 * tableRect.height() );

  double angle = angleRange(rng);
  m_v = ms_speed * tableRect.width() * QPointF(cos(angle), sin(angle));
  m_item = new PixmapItem(*ms_pixmap);
  m_item->setPos(xRange(rng), yRange(rng));
  scene.addItem(m_item);
}

Ball::~Ball()
{
  delete m_item;
}

void Ball::advance( double dt,
                    const std::vector<QLineF>& stopLines,
                    const std::vector<QPointF>& stopPoints )
{
  if ( m_isInBase )
  {
    m_timeInBase += dt;
    return;
  }
  QLineF ignoreLine;
  QPointF ignorePoint;
  unsigned int counter = 0;
  while ( dt != 0 )
  {
    if ( ++counter == 100 )
    {
      return;  // too many iterations
    }
    double vMag = QPointF::dotProduct(m_v, m_v);
    QLineF path( m_item->pos(), m_item->pos() + dt * m_v );
    QPointF firstIntersection;
    QLineF firstStopLine;
    QPointF firstStopPoint;
    for ( QLineF line : stopLines )
    {
      if ( line == ignoreLine )
      {
        continue;
      }
      QLineF lineBefore( line.p1(), path.p1() );
      QLineF lineAfter(  line.p1(), path.p2() );
      double crossBefore = line.dx() * lineBefore.dy() -
                           line.dy() * lineBefore.dx();
      double crossAfter = line.dx() * lineAfter.dy() -
                          line.dy() * lineAfter.dx();
      QPointF intersection;
      if ( crossBefore < 0 &&
           crossAfter > 0 &&
           line.intersect(path, &intersection ) == QLineF::BoundedIntersection &&
           ( firstIntersection.isNull() ||
             Utilities::dist(path.p1(), intersection) < Utilities::dist(path.p1(), firstIntersection) ) )
      {
        firstIntersection = intersection;
        firstStopLine = line;
        firstStopPoint = QPointF();
      }
    }
    for ( QPointF point : stopPoints )
    {
      if ( point == ignorePoint )
      {
        continue;
      }
      QPointF intersection = circleLineSegmentIntersection( path, point, 0.5 * ms_diameter );
      if ( !intersection.isNull() &&
           ( firstIntersection.isNull() ||
             Utilities::dist(path.p1(), intersection) < Utilities::dist(path.p1(), firstIntersection) ) )
      {
        firstIntersection = intersection;
        firstStopLine = QLineF();
        firstStopPoint = point;
      }
    }
    if ( !firstStopLine.isNull() )
    {
      QLineF l = firstStopLine.unitVector();
      l.translate(-l.p1());
      m_v = 2 * QPointF::dotProduct(m_v, l.p2()) * l.p2() - m_v; // reflect
    }
    else if ( !firstStopPoint.isNull() )
    {
      m_v = -m_v; // reflect
    }
    if ( !firstIntersection.isNull() )
    {
      m_item->setPos( firstIntersection );
      dt = dt - Utilities::dist(path.p1(), firstIntersection) / vMag;
      ignoreLine = firstStopLine;
      ignorePoint = firstStopPoint;
    }
    else
    {
      m_item->setPos( path.p2() );
      dt = 0;
    }
  }
}

QPointF Ball::getPos() const
{
  return m_item->pos();
}

void Ball::setPos(QPointF pos)
{
  m_item->setPos(pos);
}

bool Ball::isInBase() const
{
  return m_isInBase;
}

void Ball::putInBase(QPointF pos)
{
  setPos(pos);
  m_v = QPointF(0, 0);
  m_isInBase = true;
  m_timeInBase = 0;
}

bool Ball::removeMe() const
{
  return m_timeInBase > ms_timeBallsRemainsInBase;
}

double Ball::getDiameter()
{
  if ( ms_diameter < 0 )
  {
    throw std::logic_error("diameter not set, create a ball first");
  }
  return ms_diameter;
}

QPointF Ball::circleLineSegmentIntersection(QLineF line, QPointF center, double radius)
{
  QPointF d = line.p2() - line.p1();
  QPointF f = line.p1() - center;
  double a = QPointF::dotProduct(d, d);
  double b = 2*QPointF::dotProduct(f, d) ;
  double c = QPointF::dotProduct(f, f) - radius*radius ;

  double discriminant = b*b-4*a*c;
  if( discriminant < 0 )
  {
    // no intersection
    return QPointF();
  }

  discriminant = sqrt( discriminant );

  double t1 = (-b - discriminant)/(2*a);
  double t2 = (-b + discriminant)/(2*a);

  if( t1 >= 0 && t1 <= 1 )
  {
    // t1 is the intersection, and it's closer than t2
    // (since t1 uses -b - discriminant)
    // Impale, Poke
    return line.p1() + t1 * d;
  }

  // here t1 didn't intersect so we are either started
  // inside the sphere or completely past it
  if( t2 >= 0 && t2 <= 1 )
  {
    // ExitWound
    return QPointF()/*line.p1() + t2 * d*/;
  }

  // no intn: FallShort, Past, CompletelyInside
  return QPointF();
}

