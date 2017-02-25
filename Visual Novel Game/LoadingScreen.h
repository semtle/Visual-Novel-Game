#pragma once

#include <Bengine/IGameScreen.h>
#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/GLTexture.h>
#include <Bengine/SpriteBatch.h>
#include <vector>
#include <SDL/SDL.h>

#include "Indices.h"

class LoadingScreen : public Bengine::IGameScreen
{
public:
    LoadingScreen(Bengine::Window* window, const std::vector<std::string>& filePaths);
    ~LoadingScreen();

    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    virtual void build() override;
    virtual void destroy() override;
    virtual void onEntry() override;
    virtual void onExit() override;
    virtual void update() override;
    virtual void draw() override;

private:
    void loadCurrentImage();
    void checkInput();
    void initShaders();
    void loadImages();

    int m_nextScreen = SCREEN_INDEX_MAIN_MENU; ///< By default set next screen to be the main menu
    float m_loadingPercent = 0.0f;
    unsigned int m_currentLoadingIndex = 0;
    unsigned int m_dotCount = 0;

    bool m_standingAnimation = true;
    bool m_shouldAnimate = false;

    Uint32 m_lastAnimationTicks;
    Uint32 m_currentTicks;

    std::vector<std::string> m_filePaths;

    Bengine::Window* m_window;
    Bengine::Camera2D m_camera;
    Bengine::SpriteBatch m_spriteBatch;
    Bengine::GLSLProgram m_textureProgram;

    Bengine::GLTexture m_chibiLeft1, m_chibiLeft2;
    Bengine::GLTexture m_chibiRight1, m_chibiRight2;
    Bengine::GLTexture m_loadingText;
    Bengine::GLTexture m_dot;
};

