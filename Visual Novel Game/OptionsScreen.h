#pragma once

#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>

const int NO_BALL = -1;

class OptionsScreen
{
public:
    OptionsScreen();
    ~OptionsScreen();

    void init(Bengine::Window* window);
    void draw(Bengine::SpriteBatch* spriteBatch);
    void onMouseDown(float x, float y);
    void onMouseUp();
    void onMouseMove(float x, float y);

    void openOptions() { m_shouldClose = false; }

    void setHasAutoPlay(bool h) { m_autoPlay = h; }
    void setShouldClose(bool s) { m_shouldClose = s; }
    void setShouldUpdate(bool s) { m_shouldUpdate = s; }
    void setVolume(float v);
    void setTextSpeed(float s);
    void setAutoPlaySpeed(float s);

    bool  hasAutoPlay()      const { return m_autoPlay; }
    bool  shouldClose()      const { return m_shouldClose; }
    bool  shouldUpdate()     const { return m_shouldUpdate; }
    float getVolume()        const { return m_volume; }
    float getTextSpeed()     const { return m_textSpeed; }
    float getAutoPlaySpeed() const { return m_autoPlaySpeed; }
private:
    bool isMouseOnSliderBall(int idx, float mouseX, float mouseY);
    glm::vec4 getInputDimensions(glm::vec4 destRect);

    float m_volume = 0.0f;
    float m_textSpeed = 0.0f;
    float m_autoPlaySpeed = 0.0f;

    int m_grabbedBall = NO_BALL;
    int m_volumeBallPos = 0;
    bool m_shouldClose = false;
    bool m_shouldUpdate = false;
    bool m_autoPlay = false;

    int m_ballMinX, m_ballMaxX;

    glm::vec2 m_grabOffset;
    std::vector<glm::vec2> m_sliderBallPositions;

    float m_volumeSliderValue = -1;

    glm::vec2 m_backButtonSize;
    glm::vec2 m_sliderSize;
    glm::vec4 m_backButtonDestRect, m_volumeSliderDestRect, m_autoPlaySliderDestRect, m_textSpeedSliderDestRect, m_checkBoxDestRect;

    Bengine::Window* m_window;
};