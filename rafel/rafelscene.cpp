#include "rafelscene.h"
#include "pixmapitem.h"
#include "utilities.h"
#include <QPixmap>

RafelScene::RafelScene()
 : m_train(0),
   m_wagons()
{

}

void RafelScene::init()
{
  QPixmap trainPix("../trafel/resources/train_C0.png");
  QPixmap wagonPix("../trafel/resources/carbon-1293183_960_720_small_C0.png");
  trainPix = trainPix.scaledToWidth(itemSize());
  wagonPix = wagonPix.scaledToWidth(itemSize());
  m_train = new PixmapItem(trainPix);
  addItem(m_train);
  for ( unsigned int i = 0; i != 3; ++i )
  {
    m_wagons.push_back( new PixmapItem(wagonPix) );
    addItem(m_wagons.back());
  }

}

void RafelScene::eventClick(QPointF /*p*/, PointerEvent::Color /*c*/)
{

}

void RafelScene::step(unsigned int turn)
{
  double dt = 1 / getFps();
  double angleSpeed = 0.4;
  double trainAngle = angleSpeed * turn * dt;
  QRectF rect = getTableRect();
  double armX = 0.3 * rect.width();
  double armY = 0.3 * rect.height();
  m_train->setPos( rect.center() + QPointF( armX  * cos(trainAngle),
                                            -armY * sin(trainAngle) ) );
  m_train->setRotation(-90 - 180 * trainAngle / M_PI);
  double angleLength = m_train->boundingRect().width() /
                       Utilities::dist( m_train->pos(), rect.center() );
  for ( unsigned int i = 0; i != m_wagons.size(); ++i )
  {
    double angle = trainAngle - (i+1) * angleLength;
    m_wagons[i]->setPos( rect.center() + QPointF( armX  * cos(angle), -armY * sin(angle) ) );
    m_wagons[i]->setRotation(-90 - 180 * angle / M_PI);

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

