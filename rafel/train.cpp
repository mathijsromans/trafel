#include "train.h"

#include "pixmapitem.h"
#include "rafelitem.h"
#include "rafelscene.h"
#include "utilities.h"

Train::Train(RafelScene& scene)
  : m_scene(scene),
    m_angle(0),
    m_train(0)
{
  double itemSize = 0.1 * scene.getTableRect().width();
  QPixmap trainPix("../trafel/resources/train_C0.png");
  QPixmap wagonPix("../trafel/resources/carbon-1293183_960_720_small_C0.png");
  trainPix = trainPix.scaledToWidth(itemSize);
  wagonPix = wagonPix.scaledToWidth(itemSize);
  m_train = new PixmapItem(trainPix);
  scene.addItem(m_train);
  for ( unsigned int i = 0; i != 3; ++i )
  {
    m_wagons.push_back( {new PixmapItem(wagonPix), {}} );
    scene.addItem(m_wagons.back().wagon);
  }
}

Train::~Train()
{
  delete m_train;
  for ( auto w : m_wagons )
  {
    delete w.wagon;
  }

}

void Train::setAngle(double trainAngle)
{
  m_angle = trainAngle;
  QRectF rect = m_scene.getTableRect();
  double armX = 0.3 * rect.width();
  double armY = 0.3 * rect.height();
  m_train->setPos( m_scene.position(trainAngle) );
  QPointF v( armX  * sin(trainAngle), armY * cos(trainAngle) );
  double vmag = Utilities::dist(QPointF(0,0), v) / Utilities::dist(m_train->pos(), rect.center());
  m_train->setRotation(90 - 180 * trainAngle / M_PI);

  //totally wrong, but I don't have time to fix it
  double angleLength = m_train->boundingRect().width() / (sqrt(vmag) * Utilities::dist(m_train->pos(), rect.center()));

  for ( unsigned int i = 0; i != m_wagons.size(); ++i )
  {
    double angle = trainAngle - (i+1) * angleLength;
    m_wagons[i].wagon->setPos( m_scene.position(angle) );
    m_wagons[i].wagon->setRotation(90 - 180 * angle / M_PI);
    for ( unsigned int j = 0; j != m_wagons[i].items.size(); ++j )
    {
      double itemAngle = angle + j*0.2 * angleLength;
      m_wagons[i].items[j]->setPos(m_scene.position(itemAngle) );
    }

  }
}

void Train::addItem(RafelItem* item, unsigned int wagon)
{
  m_wagons[wagon].items.push_back(item);
  m_scene.addItem(item);
}

double Train::angle() const
{
    return m_angle;
}

