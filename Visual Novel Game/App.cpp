#include "App.h"
#include "LoadingFilesFetcher.h"
#include <Bengine/ScreenList.h>

App::App()
{
}

App::~App()
{
}

void App::onInit()
{
    m_audioEngine.init();
    m_menuSong = m_audioEngine.loadMusic("Audio/Music/MenuSong.mp3");
}

void App::addScreens()
{
    m_menuLoadingScreen = std::make_unique<LoadingScreen>(&m_window, LoadingFilesFetcher::getMenuLoadingScreenImagePaths());
    m_mainMenuScreen = std::make_unique<MainMenuScreen>(&m_window, this);
    m_mainGameScreen = std::make_unique<MainGameScreen>(&m_window);
    m_newGameScreen = std::make_unique<NewGameScreen>(&m_window, m_mainGameScreen.get(), this);

    m_screenList->addScreen(m_menuLoadingScreen.get());
    m_screenList->addScreen(m_mainMenuScreen.get());
    m_screenList->addScreen(m_newGameScreen.get());
    m_screenList->addScreen(m_mainGameScreen.get());
    
    m_screenList->setScreen(m_menuLoadingScreen->getScreenIndex());
}

void App::onExit()
{

}

void App::startMenuSong()
{
    m_menuSong.play(-1);
}

void App::endMenuSong()
{
    m_menuSong.stop();
}
