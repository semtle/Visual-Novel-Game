#include "OptionsScreen.h"

const int SLIDER_WIDTH = 255;
const int VOLUME_BALL = 0;
const int TEXT_BALL = 1;
const int AUTOPLAY_BALL = 2;

const int SLIDER_BALL_RADIUS = 24;

OptionsScreen::OptionsScreen()
{
}

OptionsScreen::~OptionsScreen()
{
    // Empty
}

void OptionsScreen::init(Bengine::Window* window)
{
    m_window = window;

    m_backButtonSize = glm::vec2(199, 60);
    m_sliderSize = glm::vec2(468, 75);

    m_backButtonDestRect = glm::vec4(-m_backButtonSize.x / 2, -m_window->getScreenHeight() / 2 + 35, m_backButtonSize);
    m_volumeSliderDestRect = glm::vec4(-m_sliderSize.x / 2, -m_window->getScreenHeight() / 2 + 375, m_sliderSize);
    m_textSpeedSliderDestRect = glm::vec4(-m_sliderSize.x / 2, -m_window->getScreenHeight() / 2 + 275, m_sliderSize);
    m_autoPlaySliderDestRect = glm::vec4(-m_sliderSize.x / 2, -m_window->getScreenHeight() / 2 + 175, m_sliderSize);
    m_checkBoxDestRect = glm::vec4(-m_window->getScreenWidth() / 2 + 265, -m_window->getScreenHeight() / 2 + 146, 50, 46);

    m_ballMinX = -m_window->getScreenWidth() / 2 + 443 - SLIDER_BALL_RADIUS / 2;
    m_ballMaxX = -m_window->getScreenWidth() / 2 + 690 - SLIDER_BALL_RADIUS / 2;

    m_sliderBallPositions.resize(3);

    m_sliderBallPositions[VOLUME_BALL].x = m_ballMinX;
    m_sliderBallPositions[VOLUME_BALL].y = -m_window->getScreenHeight() / 2 + 434 - SLIDER_BALL_RADIUS / 2;

    m_sliderBallPositions[TEXT_BALL].x = m_ballMinX;
    m_sliderBallPositions[TEXT_BALL].y = -m_window->getScreenHeight() / 2 + 302 - SLIDER_BALL_RADIUS / 2;

    m_sliderBallPositions[AUTOPLAY_BALL].x = m_ballMinX;
    m_sliderBallPositions[AUTOPLAY_BALL].y = -m_window->getScreenHeight() / 2 + 169 - SLIDER_BALL_RADIUS / 2;
}

void OptionsScreen::draw(Bengine::SpriteBatch* spriteBatch)
{

    static const Bengine::ColorRGBA8 color(255, 255, 255, 255);
    const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    // Background
    {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2, m_window->getScreenWidth(), m_window->getScreenHeight());

        spriteBatch->draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Backgrounds/Main Menu/OptionsScreen.png").id,
            0.0f,
            color
        );
    }

    { // Back button
        spriteBatch->draw(
            m_backButtonDestRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/DarkBackButton.png").id,
            0.0f,
            color
        );
    }

    const static Bengine::GLTexture dot = Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/SliderBall.png");

    { // Volume slider dot
        glm::vec4 destRect(m_sliderBallPositions[VOLUME_BALL].x, m_sliderBallPositions[VOLUME_BALL].y, SLIDER_BALL_RADIUS, SLIDER_BALL_RADIUS);
        
        spriteBatch->draw(
            destRect,
            uvRect,
            dot.id,
            0.0f,
            color
        );
    }

    { // Text speed slider dot
        glm::vec4 destRect(m_sliderBallPositions[TEXT_BALL].x, m_sliderBallPositions[TEXT_BALL].y, SLIDER_BALL_RADIUS, SLIDER_BALL_RADIUS);

        spriteBatch->draw(
            destRect,
            uvRect,
            dot.id,
            0.0f,
            color
        );
    }

    { // Checkbox
        spriteBatch->draw(
            m_checkBoxDestRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/OptionCheckmarkBox.png").id,
            0.0f,
            color
        );
    }

    // Checkmark
    if (m_autoPlay) {
        spriteBatch->draw(
            glm::vec4(m_checkBoxDestRect.x + 6, m_checkBoxDestRect.y + 4, 40, 40),
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/OptionCheckmark.png").id,
            0.0f,
            color
        );
    }

    // Autoplay slider
    if (m_autoPlay) {
        glm::vec4 destRect(m_ballMinX, m_sliderBallPositions[AUTOPLAY_BALL].y + SLIDER_BALL_RADIUS / 2 - 2, 255, 4);

        spriteBatch->draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/Slider.png").id,
            0.0f,
            color
        );
    }

    // Autoplay speed slider dot
    if (m_autoPlay) {
        glm::vec4 destRect(m_sliderBallPositions[AUTOPLAY_BALL].x, m_sliderBallPositions[AUTOPLAY_BALL].y, SLIDER_BALL_RADIUS, SLIDER_BALL_RADIUS);

        spriteBatch->draw(
            destRect,
            uvRect,
            dot.id,
            0.0f,
            color
        );
    }
}

void OptionsScreen::onMouseDown(float x, float y)
{
    glm::vec2 mouseCoords(x, y);

    for (size_t i = 0; i < m_sliderBallPositions.size(); i++) {
        if (isMouseOnSliderBall(i, x, y)) {
            if (!(i == AUTOPLAY_BALL && !m_autoPlay)) {
                m_grabbedBall = i;
                m_grabOffset = mouseCoords - m_sliderBallPositions[i];
                break;
            }
        }
    }

    glm::vec4 dims = getInputDimensions(m_backButtonDestRect);
    if (mouseCoords.x > dims.x && mouseCoords.x < dims.x + dims.z) {
        if (mouseCoords.y > dims.y && mouseCoords.y < dims.y + dims.a) {
            m_shouldClose = true;
        }
    }

    dims = getInputDimensions(m_checkBoxDestRect);
    if (mouseCoords.x > dims.x && mouseCoords.x < dims.x + dims.z) {
        if (mouseCoords.y > dims.y && mouseCoords.y < dims.y + dims.a) {
            m_autoPlay = !m_autoPlay;
        }
    }
}

void OptionsScreen::onMouseUp()
{
    m_grabbedBall = NO_BALL;
}

void OptionsScreen::onMouseMove(float x, float y)
{
    glm::vec2 mouseCoords(x, y);
    std::cout << "Mouse X: " << mouseCoords.x << "\n";
    std::cout << "Mouse Y: " << mouseCoords.y << "\n";

    if (m_grabbedBall != NO_BALL) {
        m_shouldUpdate = true;

        int newX = (mouseCoords - m_grabOffset).x;

        if (newX >= m_ballMinX && newX <= m_ballMaxX)
            m_sliderBallPositions[m_grabbedBall].x = newX;
        else if (newX < m_ballMinX)
            m_sliderBallPositions[m_grabbedBall].x = m_ballMinX;
        else
            m_sliderBallPositions[m_grabbedBall].x = m_ballMaxX;

        if (m_grabbedBall == VOLUME_BALL) {
            m_volume = (static_cast<float>(((float)m_sliderBallPositions[VOLUME_BALL].x - (float)m_ballMinX) / (float)SLIDER_WIDTH));
        }
        else if (m_grabbedBall == TEXT_BALL) {
            m_textSpeed = (static_cast<float>(((float)m_sliderBallPositions[TEXT_BALL].x - (float)m_ballMinX) / (float)SLIDER_WIDTH));
        }
        else {
            m_autoPlaySpeed = (static_cast<float>(((float)m_sliderBallPositions[AUTOPLAY_BALL].x - (float)m_ballMinX) / (float)SLIDER_WIDTH));
            std::cout << "Autoplay speed: " << m_autoPlaySpeed << "\n";
        }
    }
}

void OptionsScreen::setVolume(float v)
{
    m_volume = v;
    m_sliderBallPositions[VOLUME_BALL].x = m_ballMinX + v * SLIDER_WIDTH;
}

void OptionsScreen::setTextSpeed(float s)
{
    m_textSpeed = s;
    m_sliderBallPositions[TEXT_BALL].x = m_ballMinX + m_textSpeed * SLIDER_WIDTH;
}

void OptionsScreen::setAutoPlaySpeed(float s)
{
    m_autoPlaySpeed = s;
    m_sliderBallPositions[AUTOPLAY_BALL].x = m_ballMinX + m_autoPlaySpeed * SLIDER_WIDTH;
}

bool OptionsScreen::isMouseOnSliderBall(int idx, float mouseX, float mouseY)
{
    glm::vec4 pos = getInputDimensions(glm::vec4(m_sliderBallPositions[idx].x, m_sliderBallPositions[idx].y, SLIDER_BALL_RADIUS, SLIDER_BALL_RADIUS));
    int xPos = pos.x + SLIDER_BALL_RADIUS / 2;
    int yPos = pos.y + SLIDER_BALL_RADIUS / 2;

    return (mouseX >= xPos - SLIDER_BALL_RADIUS / 2.0f && mouseX < xPos + SLIDER_BALL_RADIUS / 2.0f &&
            mouseY >= yPos - SLIDER_BALL_RADIUS / 2.0f && mouseY < yPos + SLIDER_BALL_RADIUS / 2.0f);
}

glm::vec4 OptionsScreen::getInputDimensions(glm::vec4 destRect)
{
    destRect.x = destRect.x + m_window->getScreenWidth() / 2;

    // Flips the y value
    destRect.y = abs((destRect.y + m_window->getScreenHeight() / 2) - m_window->getScreenHeight()) - destRect.a;

    return destRect;
}
