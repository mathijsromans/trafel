#include "button.h"
#include <QPushButton>

Button::Button(const std::string& text)
 : QGraphicsProxyWidget(),
   m_pushButton(0)
{
  m_pushButton = new QPushButton(QString::fromStdString(text));
  setWidget(m_pushButton);
}

void Button::mouseClick(const PointerEvent::CPoint& /*p*/)
{
  emit pressed();
}



