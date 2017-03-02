#include "NewGameScreen.h"
#include "Indices.h"
#include "App.h"
#include <Bengine/IMainGame.h>

NewGameScreen::NewGameScreen(Bengine::Window* window, MainGameScreen* mainGameScreen, App* app) :
    m_window(window), m_mainGameScreen(mainGameScreen), m_app(app)
{
    m_screenIndex = SCREEN_INDEX_NEW_GAME;
}

NewGameScreen::~NewGameScreen()
{
}

int NewGameScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_PLAY;
}

int NewGameScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_MAIN_MENU;
}

void NewGameScreen::build()
{

}

void NewGameScreen::destroy()
{

}

void NewGameScreen::onEntry()
{
    // Initialize camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    // Init spritebatch
    m_spriteBatch.init();

    // Init font
    m_spriteFont = std::make_unique<Bengine::SpriteFont>("Fonts/cabin-condensed.regular.ttf", 32);

    // Initialize shaders
    initShaders();
}

void NewGameScreen::onExit()
{
    m_spriteBatch.dispose();
    m_textureProgram.dispose();
    m_spriteFont->dispose();
}

void NewGameScreen::update()
{
    m_camera.update();
    checkInput();
}

void NewGameScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_textureProgram.use();
    m_spriteBatch.begin(Bengine::GlyphSortType::NONE);

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    static const Bengine::ColorRGBA8 color(255, 255, 255, 255);
    static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    // Start screen
    {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2, m_window->getScreenWidth(), m_window->getScreenHeight());

        m_spriteBatch.draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Backgrounds/Main Menu/entername.png").id,
            0.0f,
            color
        );
    }

    { // Player Name
        m_spriteFont->draw(
            m_spriteBatch,
            m_playerName.c_str(),
            glm::vec2(0, 18),
            glm::vec2(1.0f),
            1.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255),
            Bengine::Justification::MIDDLE
        );
    }

    // New game button
    if (isPlayerNameValid()) {
        static const glm::vec2 NEWGAME_DIMENSIONS(195, 38);
        glm::vec4 destRect(-NEWGAME_DIMENSIONS.x / 2, -98, NEWGAME_DIMENSIONS);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/newgame.png").id,
            0.0f,
            color
        );
    }

    { // Back button
        glm::vec4 destRect(-51, -m_window->getScreenHeight() / 2 + 46, 102, 26);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/backbutton.png").id,
            0.0f,
            color
        );
    }

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    m_textureProgram.unuse();
}

void NewGameScreen::checkInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        m_game->onSDLEvent(event);

        switch (event.type) {
        case SDL_QUIT:
            m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
            break;
        case SDL_TEXTINPUT:
            if (m_playerName.length() < 16) m_playerName += event.text.text;
            break;
        case SDL_KEYDOWN:
            // Doing this here because outside here would not work properly
            if (event.key.keysym.sym == SDLK_BACKSPACE)
                m_playerName = m_playerName.substr(0, m_playerName.length() - 1);
            break;
        }
    }

    if (m_game->inputManager.isKeyPressed(SDLK_RETURN) && isPlayerNameValid()) {
        m_currentState = Bengine::ScreenState::CHANGE_NEXT;
        m_mainGameScreen->setPlayerName(m_playerName);
        m_app->endMenuSong();
    }

    glm::vec2 mouseCoords = m_game->inputManager.getMouseCoords();

    // Check mouse inputs on the 2 buttons
    if (m_game->inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
        // New game
        if (mouseCoords.x > 302 && mouseCoords.x < 497 && isPlayerNameValid()) {
            if (mouseCoords.y > 360 && mouseCoords.y < 398) {
                m_currentState = Bengine::ScreenState::CHANGE_NEXT;
                m_mainGameScreen->setPlayerName(m_playerName);
                m_app->endMenuSong();
            }
        }

        // Back
        if (mouseCoords.x > 349 && mouseCoords.x < 451) {
            if (mouseCoords.y > 528 && mouseCoords.y < 554) {
                m_currentState = Bengine::ScreenState::CHANGE_PREVIOUS;
            }
        }
    }
}

void NewGameScreen::initShaders()
{
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

bool NewGameScreen::isPlayerNameValid()
{
    return (m_playerName.length() >= 3 && m_playerName.length() <= 16);
}
