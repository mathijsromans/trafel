#ifndef TRANSFORMSCENE_H
#define TRANSFORMSCENE_H

#include "calibration.h"
#include "pointerevent.h"
#include <QGraphicsScene>
#include <QPoint>
#include <vector>

class Button;
class QGraphicsPixmapItem;
class QGraphicsTextItem;

class TransformScene : public QGraphicsScene
{
  Q_OBJECT

public:
  TransformScene();
  virtual ~TransformScene();
  void calibrate();
  virtual void inputEvent(const PointerEvent& e);
  void doInit();
  void showInfoText(const std::string& text);

signals:
  void quit();

public slots:
  void slotLightAt( PointerEvent e );
  void slotQuit();
  void slotQuitYes();
  void slotQuitNo();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  QRectF getTableRect() const;

private:
  virtual void init() = 0;
  virtual void mouseClick(QPointF p);

private:
  Calibration m_calibration;
  QGraphicsTextItem* m_infoText;
  Button* m_quitYes;
  Button* m_quitNo;
};

#endif // TRANSFORMSCENE_H
