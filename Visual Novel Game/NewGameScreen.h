#pragma once

#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>

#include "MainGameScreen.h"

#include <memory>

class App;

class NewGameScreen : public Bengine::IGameScreen
{
public:
    NewGameScreen(Bengine::Window* window, MainGameScreen* mainGameScreen, App* app);
    ~NewGameScreen();

    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    virtual void build() override;
    virtual void destroy() override;
    virtual void onEntry() override;
    virtual void onExit() override;
    virtual void update() override;
    virtual void draw() override;

private:
    void checkInput();
    void initShaders();
    bool isPlayerNameValid();

    int m_nextScreen = SCREEN_INDEX_NO_SCREEN;
    std::string m_playerName = "";

    MainGameScreen* m_mainGameScreen;

    App* m_app;
    Bengine::Window* m_window;
    Bengine::Camera2D m_camera;
    Bengine::SpriteBatch m_spriteBatch;
    std::unique_ptr<Bengine::SpriteFont> m_spriteFont = nullptr;
    Bengine::GLSLProgram m_textureProgram;
};