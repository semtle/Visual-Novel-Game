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

}

void App::addScreens()
{
    m_menuLoadingScreen = std::make_unique<LoadingScreen>(&m_window, LoadingFilesFetcher::getMenuLoadingScreenImagePaths());
    m_mainMenuScreen = std::make_unique<MainMenuScreen>(&m_window);
    m_newGameScreen = std::make_unique<NewGameScreen>(&m_window);

    m_screenList->addScreen(m_menuLoadingScreen.get());
    m_screenList->addScreen(m_mainMenuScreen.get());
    m_screenList->addScreen(m_newGameScreen.get());
    
    m_screenList->setScreen(m_menuLoadingScreen->getScreenIndex());
}

void App::onExit()
{

}
