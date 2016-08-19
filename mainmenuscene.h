#ifndef MAINMENUSCENE_H
#define MAINMENUSCENE_H

#include "transformscene.h"
#include <array>
#include <string>

class MainMenuScene : public TransformScene
{
  Q_OBJECT

public:
  enum class Games { mainMenu, poker, trafel, spafel, flafel, rafel, MAX };
  const std::array<std::string, static_cast<unsigned int>(Games::MAX)> m_gameNames;
  MainMenuScene();
signals:
  void signalStartGame(MainMenuScene::Games game);
  void signalChangeNumPlayers(int numPlayers);
private slots:
  void slotGameButtonPressed(int button);
  void slotChangeNumPlayers(int numPlayers);
private:
  void init() override;
  void createGameButtons();
  void createNumPlayerButtons();
  void updateText();
private:
  QGraphicsTextItem* m_numPlayerText;
};

#endif // MAINMENUSCENE_H
