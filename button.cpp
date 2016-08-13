#include "button.h"
#include <QDebug>
#include <QPushButton>

Button::Button(const std::string& text)
 : QGraphicsProxyWidget(),
   m_pushButton(0)
{
  m_pushButton = new QPushButton(QString::fromStdString(text));
  QFontMetrics metrics(m_pushButton->font());
  QRect textBox = metrics.boundingRect(m_pushButton->text()+ " ");
  int width = std::max<int>( 1.5 *textBox.height(), 1.5 * textBox.width() );
  m_pushButton->setMaximumWidth( width );
  setWidget(m_pushButton);
}

void Button::eventClick(const PointerEvent::CPoint& /*p*/)
{
  emit pressed();
}



