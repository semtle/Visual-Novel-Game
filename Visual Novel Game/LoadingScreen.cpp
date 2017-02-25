#include "LoadingScreen.h"
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <glm/glm.hpp>

LoadingScreen::LoadingScreen(Bengine::Window* window, const std::vector<std::string>& filePaths) :
    m_window(window), m_filePaths(filePaths)
{
    // Empty
}

LoadingScreen::~LoadingScreen()
{
    // Empty
}

int LoadingScreen::getNextScreenIndex() const
{
    return m_nextScreen;
}

int LoadingScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

void LoadingScreen::build()
{
    // Empty
}

void LoadingScreen::destroy()
{
    // Empty
}

void LoadingScreen::onEntry()
{
    // Init camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    // Init spritebatch
    m_spriteBatch.init();

    // Init shaders
    initShaders();

    // Load all images that are in the loading screen
    loadImages();

    m_lastAnimationTicks = SDL_GetTicks();
    m_currentTicks = SDL_GetTicks();
}

void LoadingScreen::onExit()
{
    // Empty
}

void LoadingScreen::update()
{
    loadCurrentImage();

    m_camera.update();
    checkInput();

    printf("%.2f\n", m_loadingPercent); // TODO: REMOVE

    // Update if animation be ran
    m_currentTicks = SDL_GetTicks();
    if (m_currentTicks - m_lastAnimationTicks >= 700) {
        m_lastAnimationTicks = SDL_GetTicks();
        m_shouldAnimate = true;
        m_standingAnimation = !m_standingAnimation;

        // Add a dot or remove them if there's 3
        m_dotCount += (m_dotCount == 3) ? -3 : 1;
    }
    else {
        m_shouldAnimate = false;
    }

    if (m_currentLoadingIndex >= m_filePaths.size()) {
        m_currentState = Bengine::ScreenState::CHANGE_NEXT;
        puts("Done loading!\n");
    }
}

void LoadingScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

    m_textureProgram.use();

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    { // Loading text
        glm::vec4 destRect(-198, 134, 396, 124);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            m_loadingText.id,
            0.0f,
            color
        );
    }

    { // Loading dot
        glm::vec4 destRect(163, 134, 68, 68);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        for (size_t i = 0; i < m_dotCount; i++) {
            m_spriteBatch.draw(
                destRect,
                uvRect,
                m_dot.id,
                0.0f,
                color
            );

            destRect.x += 40;
        }
    }

    { // Left Chibi
        glm::vec4 destRect(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2, 288, 400);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        Bengine::GLTexture texture = (m_standingAnimation) ? m_chibiLeft1 : m_chibiLeft2;
        if (!m_standingAnimation) destRect.z = 309;

        m_spriteBatch.draw(
            destRect,
            uvRect,
            texture.id,
            0.0f,
            color
        );
    }

    { // Right Chibi
        glm::vec4 destRect(m_window->getScreenWidth() / 2 - 288, -m_window->getScreenHeight() / 2, 288, 400);
        glm::vec4 uvRect(0.0f, 0.0f, -1.0f, 1.0f);

        Bengine::GLTexture texture;

        if (!m_standingAnimation) {
            destRect.z = 309;
            destRect.x -= 21;
            texture = m_chibiRight2;
        }
        else {
            texture = m_chibiRight1;
        }

        m_spriteBatch.draw(
            destRect,
            uvRect,
            texture.id,
            0.0f,
            color
        );
    }

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();
}

void LoadingScreen::loadCurrentImage()
{
    Bengine::ResourceManager::getTexture(m_filePaths[m_currentLoadingIndex]);

    m_loadingPercent = (float)m_currentLoadingIndex / (float)m_filePaths.size() * 100.0f;

    m_currentLoadingIndex++;
}

void LoadingScreen::checkInput()
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

void LoadingScreen::initShaders()
{
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void LoadingScreen::loadImages()
{
    // Left chibi
    m_chibiLeft1 = Bengine::ResourceManager::getTexture("Textures/Characters/Chibi/chibiBlue1.png");
    m_chibiLeft2 = Bengine::ResourceManager::getTexture("Textures/Characters/Chibi/chibiBlue2.png");

    // Right chibi
    m_chibiRight1 = Bengine::ResourceManager::getTexture("Textures/Characters/Chibi/chibiAlt1.png");
    m_chibiRight2 = Bengine::ResourceManager::getTexture("Textures/Characters/Chibi/chibiAlt2.png");

    // Loading text and dot
    m_loadingText = Bengine::ResourceManager::getTexture("Textures/Visuals/LoadingText.PNG");
    m_dot = Bengine::ResourceManager::getTexture("Textures/Visuals/LoadingDot.png");
}
