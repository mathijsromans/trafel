#include "mainmenuscene.h"

#include "button.h"
#include <QSignalMapper>

MainMenuScene::MainMenuScene()
   : TransformScene(),
     m_gameNames({"MAIN MENU", "POKER", "TRAFEL", "SPAFEL", "FLAFEL", "RAFEL"})
{
}

void MainMenuScene::slotButtonPressed(int button)
{
  emit signalStartGame( Games(button) );
}

void MainMenuScene::init()
{
  const double buttonSpacing = 1.5;
  QSignalMapper* signalMapper = new QSignalMapper(this);
  QRectF tableRect = getTableRect();
  double totalHeight = 0;
  std::array<Button*, static_cast<unsigned int>(Games::MAX)> buttons;
  for ( unsigned int i = 1; i != m_gameNames.size(); ++i )
  {
    buttons[i] = new Button(m_gameNames[i]);
    totalHeight += buttonSpacing * buttons[i]->boundingRect().height();
  }
  double currentOffsetY = -0.5 * totalHeight;
  for ( unsigned int i = 1; i != m_gameNames.size(); ++i )
  {
    QRectF buttonRect = buttons[i]->boundingRect();
    QPointF buttonOffset = buttonRect.bottomRight() - buttonRect.topLeft();
    buttons[i]->setPos(tableRect.center() + QPointF(0, currentOffsetY) - 0.5*buttonOffset );
    currentOffsetY += buttonSpacing * buttonRect.height();
    connect(buttons[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
    signalMapper->setMapping(buttons[i], i );
    addItem( buttons[i] );
  }
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(slotButtonPressed(int)));
}

