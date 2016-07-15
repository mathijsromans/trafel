#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsProxyWidget>
#include "clickableitem.h"

class QPushButton;

class Button : public QGraphicsProxyWidget, public ClickableItem
{
  Q_OBJECT

public:
  explicit Button(const std::string& text);
  virtual void mouseClick(const PointerEvent::CPoint& p);
signals:
  void pressed();
private:
  QPushButton* m_pushButton;

};

#endif // BUTTON_H
