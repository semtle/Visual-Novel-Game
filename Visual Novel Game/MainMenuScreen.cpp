#include "MainMenuScreen.h"
#include "Indices.h"
#include "App.h"
#include "OptionsScreen.h"
#include <Bengine/IMainGame.h>
#include <fstream>

MainMenuScreen::MainMenuScreen(Bengine::Window* window, App* app) :
    m_window(window), m_app(app)
{
    m_screenIndex = SCREEN_INDEX_MAIN_MENU;
}

MainMenuScreen::~MainMenuScreen()
{
}

int MainMenuScreen::getNextScreenIndex() const
{
    return m_nextScreen;
}

int MainMenuScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::build()
{
    m_optionsScreen.init(m_window);
    loadOptions();
}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{
    // Initialize camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    // Init spritebatch
    m_spriteBatch.init();

    // Initialize shaders
    initShaders();

    if (!m_isBuilt) {
        m_app->startMenuSong();
        m_isBuilt = true;
    }
}

void MainMenuScreen::onExit()
{
    m_spriteBatch.dispose();
    m_textureProgram.dispose();
    saveOptions();
}

void MainMenuScreen::update()
{
    m_camera.update();
    checkInput();

    if (m_optionsScreen.shouldUpdate()) {
        m_app->setMusicVolume(m_optionsScreen.getVolume());
        m_optionsScreen.setShouldUpdate(false);
    }

    if (m_optionsScreen.shouldClose()) m_showOptions = false;
}

void MainMenuScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_textureProgram.use();
    m_spriteBatch.begin();

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

    // Start screen
    {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2, m_window->getScreenWidth(), m_window->getScreenHeight());
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Backgrounds/Main Menu/mainscreen.png").id,
            0.0f,
            color
        );
    }

    if (m_showOptions) {
        m_optionsScreen.draw(&m_spriteBatch);
    }

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    m_textureProgram.unuse();
}

void MainMenuScreen::checkInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        m_game->onSDLEvent(event);

        switch (event.type) {
        case SDL_QUIT:
            m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (m_showOptions)
                m_optionsScreen.onMouseDown((float)event.button.x, (float)event.button.y);
            break;
        case SDL_MOUSEBUTTONUP:
            if (m_showOptions)
                m_optionsScreen.onMouseUp();
            break;
        case SDL_MOUSEMOTION:
            if (m_showOptions)
                m_optionsScreen.onMouseMove((float)event.motion.x, (float)event.motion.y);
            break;
        }
    }

    glm::vec2 mouseCoords = m_game->inputManager.getMouseCoords();

    if (m_game->inputManager.isKeyPressed(SDL_BUTTON_LEFT) && !m_showOptions) {
        // All buttons are at the same x position and have same width
        if (mouseCoords.x > 305 && mouseCoords.x < 478) {
            // New game
            if (mouseCoords.y > 410 && mouseCoords.y < 453) {
                m_nextScreen = SCREEN_INDEX_NEW_GAME;
                m_currentState = Bengine::ScreenState::CHANGE_NEXT;
            }

            // Load game
            if (mouseCoords.y > 460 && mouseCoords.y < 500) {
                // TODO: Implement
            }

            // Options
            if (mouseCoords.y > 507 && mouseCoords.y < 546) {
                m_optionsScreen.openOptions();
                m_showOptions = true;
            }

            // Exit
            if (mouseCoords.y > 553 && mouseCoords.y < 593) {
                m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
            }
        }
    }
}

void MainMenuScreen::initShaders()
{
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void MainMenuScreen::loadOptions()
{
    std::ifstream file("SaveFiles/options.txt");
    if (file.fail()) {
        perror("SaveFiles/options.txt");
        return;
    }

    // Load values from the options save file
    float volume, textSpeed, autoPlaySpeed;
    file >> volume >> textSpeed >> autoPlaySpeed;

    file.close();

    m_optionsScreen.setVolume(volume);
    m_optionsScreen.setTextSpeed(textSpeed);
    m_optionsScreen.setAutoPlaySpeed(autoPlaySpeed);

    m_app->setMusicVolume(volume);
}

void MainMenuScreen::saveOptions()
{
    std::ofstream file("SaveFiles/options.txt");
    if (file.fail()) {
        perror("SaveFiles/options.txt");
        return;
    }

    file << m_optionsScreen.getVolume() << '\n';
    file << m_optionsScreen.getTextSpeed() << '\n';
    file << m_optionsScreen.getAutoPlaySpeed() << '\n';

    file.close();
}
