#pragma once

#include <Bengine/IMainGame.h>
#include <Bengine/AudioEngine.h>
#include <memory>
#include <vector>
#include <string>

#include "LoadingScreen.h"
#include "MainMenuScreen.h"
#include "NewGameScreen.h"
#include "MainGameScreen.h"

class App : public Bengine::IMainGame
{
public:
    App();
    ~App();

    virtual void onInit() override;
    virtual void addScreens() override;
    virtual void onExit() override;

    void startMenuSong();
    void endMenuSong();
private:
    Bengine::AudioEngine m_audioEngine;
    Bengine::Music m_menuSong;

    std::unique_ptr<LoadingScreen> m_menuLoadingScreen = nullptr;
    std::unique_ptr<MainMenuScreen> m_mainMenuScreen = nullptr;
    std::unique_ptr<NewGameScreen> m_newGameScreen = nullptr;
    std::unique_ptr<MainGameScreen> m_mainGameScreen = nullptr;
};