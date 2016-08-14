#ifndef MAINMENUSCENE_H
#define MAINMENUSCENE_H

#include "transformscene.h"
#include <array>
#include <string>

class MainMenuScene : public TransformScene
{
  Q_OBJECT

public:
  enum class Games { mainMenu, poker, trafel, spafel, flafel, MAX };
  const std::array<std::string, static_cast<unsigned int>(Games::MAX)> m_gameNames;
  MainMenuScene();
signals:
  void signalStartGame(MainMenuScene::Games game);
private slots:
  void slotButtonPressed(int button);
private:
  void init() override;
};

#endif // MAINMENUSCENE_H
