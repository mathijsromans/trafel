#include "mainmenuscene.h"

#include "button.h"
#include <QSignalMapper>

MainMenuScene::MainMenuScene()
   : TransformScene(),
     m_gameNames({"MAIN MENU", "POKER", "TRAFEL", "SPAFEL", "FLAFEL", "RAFEL"})
{
}

void MainMenuScene::slotGameButtonPressed(int button)
{
  emit signalStartGame( Games(button) );
}

void MainMenuScene::slotChangeNumPlayers(int numPlayers)
{
  emit signalChangeNumPlayers(numPlayers);
  updateText();
}

void MainMenuScene::createGameButtons()
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
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(slotGameButtonPressed(int)));
}

void MainMenuScene::createNumPlayerButtons()
{
  const double buttonSpacing = 1.5;
  const unsigned int maxPlayers = 8;
  QSignalMapper* signalMapper = new QSignalMapper(this);
  QRectF tableRect = getTableRect();
  double totalWidth = 0;
  std::array<Button*, maxPlayers+1> buttons;
  for ( unsigned int i = 1; i < buttons.size(); ++i )
  {
    buttons[i] = new Button(std::to_string(i));
    totalWidth += buttonSpacing * buttons[i]->boundingRect().width();
  }
  double currentOffsetY = -0.5 * totalWidth;
  for ( unsigned int i = 1; i < buttons.size(); ++i )
  {
    QRectF buttonRect = buttons[i]->boundingRect();
    QPointF buttonOffset = buttonRect.bottomRight() - buttonRect.topLeft();
    buttons[i]->setPos(0.5 * tableRect.center() +
                       0.25 * tableRect.bottomLeft() +
                       0.25 * tableRect.bottomRight() +
                       QPointF(currentOffsetY, 0) - 0.5*buttonOffset );
    currentOffsetY += buttonSpacing * buttonRect.width();
    connect(buttons[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
    signalMapper->setMapping(buttons[i], i );
    addItem( buttons[i] );
  }
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(slotChangeNumPlayers(int)));
}

void MainMenuScene::updateText()
{
  m_numPlayerText->setPlainText("Number of players: " + QString::number(getNumPlayers()));
}

void MainMenuScene::init()
{
  QRectF tableRect = getTableRect();
  m_numPlayerText = addText("");
  QFont font;
  font.setPixelSize(40);
  m_numPlayerText->setFont(font);
  m_numPlayerText->setDefaultTextColor(Qt::white);
  m_numPlayerText->setPos(0.3 * tableRect.center() +
                     0.7 * tableRect.bottomLeft() );
  m_numPlayerText->show();

  createGameButtons();
  createNumPlayerButtons();
  updateText();
}

