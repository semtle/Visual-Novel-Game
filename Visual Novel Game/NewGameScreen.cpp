#include "NewGameScreen.h"
#include "Indices.h"

NewGameScreen::NewGameScreen(Bengine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_MAIN_MENU;
}

NewGameScreen::~NewGameScreen()
{
}

int NewGameScreen::getNextScreenIndex() const
{
    return m_nextScreen;
}

int NewGameScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
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

    // Initialize shaders
    initShaders();
}

void NewGameScreen::onExit()
{

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
            Bengine::ResourceManager::getTexture("Textures/Backgrounds/Main Menu/entername.png").id,
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
        switch (event.type) {
        case SDL_QUIT:
            m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
            break;
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