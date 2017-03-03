#pragma once

#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <Bengine/SpriteFont.h>

#include "OptionsScreen.h"
#include "LoadGameScreen.h"

class App;

class MainMenuScreen : public Bengine::IGameScreen
{
public:
    MainMenuScreen(Bengine::Window* window, App* app);
    ~MainMenuScreen();

    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    virtual void build() override;
    virtual void destroy() override;
    virtual void onEntry() override;
    virtual void onExit() override;
    virtual void update() override;
    virtual void draw() override;

    OptionsScreen getOptionsScreen() const { return m_optionsScreen; }
private:
    void checkInput();
    void initShaders();
    void loadOptions();
    void saveOptions();

    int m_nextScreen = SCREEN_INDEX_NO_SCREEN;
    bool m_isBuilt = false;
    bool m_showOptions = false;
    bool m_showLoadMenu = false;
    
    App* m_app;
    LoadGameScreen m_loadGameScreen;
    OptionsScreen m_optionsScreen;
    Bengine::SpriteFont m_spriteFont;
    Bengine::Window* m_window;
    Bengine::Camera2D m_camera;
    Bengine::SpriteBatch m_spriteBatch;
    Bengine::GLSLProgram m_textureProgram;
};