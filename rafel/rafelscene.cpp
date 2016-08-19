#include "rafelscene.h"
#include "rafelitem.h"
#include "demandtrack.h"
#include "pixmapitem.h"
#include "train.h"
#include "button.h"
#include "utilities.h"
#include <QPixmap>
#include <QSignalMapper>

RafelScene::RafelScene()
  : m_status(Status::driving),
    m_atStation(0)
{
}

RafelScene::~RafelScene()
{
}

void RafelScene::init()
{
  m_train = std::make_unique<Train>(*this);
  m_train->addItem(new RafelItem(0.2*itemSize(), RafelItem::Type::Blue), 0);
  m_train->addItem(new RafelItem(0.2*itemSize(), RafelItem::Type::Blue), 1);
  m_train->addItem(new RafelItem(0.2*itemSize(), RafelItem::Type::Red), 2);
  m_train->addItem(new RafelItem(0.2*itemSize(), RafelItem::Type::Green), 2);
  m_atStation = 0;
  m_train->setAngle(stationAngle(getNumPlayers()*2-1));

  QSignalMapper* signalMapper = new QSignalMapper(this);
  for ( unsigned int i = 0; i != getNumPlayers(); ++i )
  {
    m_demands.push_back(std::make_unique<DemandTrack>(*this, stationPosition(2*i+1)));
    auto goButton = new Button("Go!");
    goButton->setPos(stationPosition(2*i));
    connect(goButton, SIGNAL(pressed()), signalMapper, SLOT(map()));
    signalMapper->setMapping(goButton, i );
    addItem(goButton);
    m_goButtons.push_back(goButton);
  }  
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(slotGoButtonPressed(int)));
}

void RafelScene::eventClick(QPointF /*p*/, PointerEvent::Color /*c*/)
{
}

void RafelScene::step(unsigned int /*turn*/)
{
  if ( m_status == Status::driving )
  {
    double newAngle = m_train->angle()+ angleStep();
    m_train->setAngle( newAngle );
    double diff = fmod(fabs(stationAngle(m_atStation) - newAngle ), 2 * M_PI);
    if ( diff < 3 * angleStep() ||
         fabs(2 *M_PI-diff) < 3 * angleStep() )
    {
      m_status = Status::stopped;
    }
  }
}

QPointF RafelScene::position(double angle) const
{
  QRectF tableRect = getTableRect();
  return tableRect.center() + QPointF(-0.5 * tableRect.width()  * cos(angle),
                                      0.5 * tableRect.height() * sin(angle) );
}

QPointF RafelScene::stationPosition(unsigned int nr) const
{
  return position( stationAngle(nr));
}

double RafelScene::stationAngle(unsigned int nr) const
{
  return 2 * M_PI * (nr+1) / (getNumPlayers()*2 + 1);
}

double RafelScene::angleStep() const
{
  double angleSpeed = 0.4;
  double dt = 1 / getFps();
  return angleSpeed * dt;
}

void RafelScene::slotGoButtonPressed(int player)
{
  if ( m_status == Status::stopped &&
       static_cast<unsigned int>( 2*player ) == m_atStation )
  {
    m_status = Status::driving;
    m_atStation = (m_atStation+1)%(getNumPlayers()*2);
  }
}

double RafelScene::getFps() const
{
  return 20;
}

double RafelScene::itemSize() const
{
  return 0.1 * getTableRect().width();
}

