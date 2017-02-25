#pragma once

#include <Bengine/IMainGame.h>
#include <memory>
#include <vector>
#include <string>

#include "LoadingScreen.h"
#include "MainMenuScreen.h"
#include "NewGameScreen.h"

class App : public Bengine::IMainGame
{
public:
    App();
    ~App();

    virtual void onInit() override;
    virtual void addScreens() override;
    virtual void onExit() override;

private:
    std::unique_ptr<LoadingScreen> m_menuLoadingScreen = nullptr;
    std::unique_ptr<MainMenuScreen> m_mainMenuScreen = nullptr;
    std::unique_ptr<NewGameScreen> m_newGameScreen = nullptr;
};