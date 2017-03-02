#include "MainGameScreen.h"
#include "Indices.h"
#include <fstream>
#include <Bengine/IMainGame.h>

const int CHAR_HEIGHT = 600;
const float MESSAGE_SCALE = 0.55f;
const int ANSWER_BOX_SPACE = 160;
const int ANSWER_BOX_WIDTH = 550;
const int ANSWER_BOX_HEIGHT = 86;

MainGameScreen::MainGameScreen(Bengine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_PLAY;
}


MainGameScreen::~MainGameScreen()
{
    // Empty
}

int MainGameScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

int MainGameScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_MAIN_MENU;
}

void MainGameScreen::build()
{
    m_optionsScreen.init(m_window);
}

void MainGameScreen::destroy()
{
    // Empty
}

void MainGameScreen::onEntry()
{
    // Initialize camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    // Init spritebatch
    m_spriteBatch.init();

    // Init font
    m_spriteFont.init("Fonts/cabin-condensed.regular.ttf", 32);

    // Init audio engine
    m_audioEngine.init();

    // Initialize shaders
    initShaders();

    // Load options
    loadOptions();

    // Initialize current day
    initDay();

    // Set the character values
    updateNewDialogueValues();
}

void MainGameScreen::onExit()
{
    m_spriteBatch.dispose();
    m_textureProgram.dispose();
    m_spriteFont.dispose();
    saveOptions();
}

void MainGameScreen::update()
{
    m_camera.update();
    checkInput();

    if (m_optionsScreen.shouldUpdate()) {
        m_audioEngine.setVolume(m_optionsScreen.getVolume());
        m_optionsScreen.setShouldUpdate(false);
    }

    if (m_optionsScreen.shouldClose()) {
        m_showOptions = false;
        m_optionsScreen.setShouldClose(false);
    }

    // Fade in
    if (m_fadeIn) {
        if (m_alphaValue < 255) {
            m_alphaValue++;

            Uint32 ticks = SDL_GetTicks();

            // Check exit input so the game doesn't freeze and player can exit
            while (SDL_GetTicks() - ticks < 10) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
                }
            }
        }
        else {
            m_fadeIn = false;

            // Release the left button because otherwise player has to click twice
            m_game->inputManager.releaseKey(SDL_BUTTON_LEFT);
        }
    }

    // Fade out
    if (m_fadingOut && !m_waitAfterClickedOption) {
        if (m_alphaValue > 0) {
            m_alphaValue--;

            Uint32 ticks = SDL_GetTicks();

            // Check exit input so the game doesn't freeze and player can exit
            while (SDL_GetTicks() - ticks < 10) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
                }
            }
        }
        else {
            m_fadingOut = false;
            m_shouldFadeOut = false;
            m_alphaValue = 255;

            // Release the left button because otherwise player has to click twice
            m_game->inputManager.releaseKey(SDL_BUTTON_LEFT);

            // Change to next dialogue
            changeToNextDialogue();
        }
    }

    if (m_firstUpdateAfterOptionClick) {
        m_firstUpdateAfterOptionClick = false;
    }
    else {
        if (m_waitAfterClickedOption) {
            Uint32 ticks = SDL_GetTicks();

            // Check exit input so the game doesn't freeze and player can exit
            while (SDL_GetTicks() - ticks <= 1250) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
                }
            }

            m_clickedGreenIdx = -1;
            m_clickedRedIdx = -1;
            m_waitAfterClickedOption = false;
            m_currentDialogueIndex = m_nextDlgFromOption;
            m_currentDialogueStr = std::to_string(m_nextDlgFromOption);
            m_nextDlgFromOption = -1;
            updateNewDialogueValues();

            // Release the left button because otherwise player has to click twice
            m_game->inputManager.releaseKey(SDL_BUTTON_LEFT);
        }
    }

    // Left char alpha
    if (m_leftCharAlpha < 255) {
        m_leftCharAlpha += 8;
        if (m_leftCharAlpha > 255) {
            m_leftCharAlpha = 255;
        }
    }

    // Right char alpha
    if (m_rightCharAlpha < 255) {
        m_rightCharAlpha += 8;
        if (m_rightCharAlpha > 255) {
            m_rightCharAlpha = 255;
        }
    }
}

void MainGameScreen::draw()
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

    drawImages();

    drawTexts();

    if (m_showOptions)
        m_optionsScreen.draw(&m_spriteBatch);

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();
}

void MainGameScreen::drawImages()
{
    static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

    { // Background
        std::string name = m_file[m_currentSceneStr]["name"].as<std::string>();

        glm::vec4 destRect(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2, m_window->getScreenWidth(), m_window->getScreenHeight());
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        std::string path = m_file[m_currentSceneStr]["Background"].as<std::string>();
        Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Textures/Backgrounds/" + path);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            texture.id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, m_alphaValue)
        );
    }

    // Last Left Character
    if (m_leftCharWidth != -1 && m_fadeLeftChar && !m_fadeIn && !m_fadingOut) {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2 + 50, -m_window->getScreenHeight() / 2, m_leftCharWidth, CHAR_HEIGHT);
        glm::vec4 uvRect(0.0f, 0.0f, -1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            m_lastLeftTexture.id,
            0.0f,
            color
        );
    }

    // Current Left Character
    if (m_leftCharWidth != -1 && !m_fadeIn && !m_fadingOut) {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2 + 50, -m_window->getScreenHeight() / 2, m_leftCharWidth, CHAR_HEIGHT);
        glm::vec4 uvRect(0.0f, 0.0f, -1.0f, 1.0f);
        std::string path = m_file[m_currentSceneStr][m_currentDialogueStr]["left"].as<std::string>();

        std::string charName = path.substr(0, path.find(","));
        std::string fileName = path.substr(path.find(", ") + 2, path.length());

        Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Textures/Characters/" + charName + "/" + fileName);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            texture.id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, m_leftCharAlpha)
        );
    }

    // Last Right Character
    if (m_rightCharWidth != -1 && m_fadeRightChar && !m_fadeIn && !m_fadingOut) {
        glm::vec4 destRect(m_window->getScreenWidth() / 2 - 300, -m_window->getScreenHeight() / 2, m_rightCharWidth, CHAR_HEIGHT);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            m_lastRightTexture.id,
            0.0f,
            color
        );
    }

    // Current Right Character
    if (m_rightCharWidth != -1 && !m_fadeIn && !m_fadingOut) {
        glm::vec4 destRect(m_window->getScreenWidth() / 2 - 300, -m_window->getScreenHeight() / 2, m_rightCharWidth, CHAR_HEIGHT);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        std::string path = m_file[m_currentSceneStr][m_currentDialogueStr]["right"].as<std::string>();

        std::string charName = path.substr(0, path.find(","));
        std::string fileName = path.substr(path.find(", ") + 2, path.length());

        Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Textures/Characters/" + charName + "/" + fileName);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            texture.id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, m_rightCharAlpha)
        );
    }

    // Talker box
    if (m_hasTalkerBox && !m_fadeIn && !m_fadingOut) {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2 + 20, -m_window->getScreenHeight() / 2 + 157, 159, 66);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/TalkerBox.png").id,
            0.0f,
            color
        );
    }

    // Dialogue box
    if (m_hasDialogueBox && !m_fadeIn && !m_fadingOut) {
        glm::vec4 destRect(-m_window->getScreenWidth() / 2 + 6, -m_window->getScreenHeight() / 2, 788, 177);
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/BlueBoxSLQFix.png").id,
            0.0f,
            color
        );
    }

    // Question boxes
    if (m_isQuestion && ((!m_fadeIn && !m_fadingOut) || (m_waitAfterClickedOption))) {
        drawAnswerBoxes(m_clickedRedIdx, m_clickedGreenIdx);
    }
}

void MainGameScreen::drawTexts()
{
    char buffer[256];
    static const glm::vec2 bottomLeft(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2);

    // Talker
    if (m_hasTalkerBox && (!m_fadeIn && !m_fadingOut)) {
        sprintf_s(buffer, "%s", m_talker.c_str());

        m_spriteFont.draw(
            m_spriteBatch,
            buffer,
            bottomLeft + glm::vec2(98, 182),
            glm::vec2(0.6f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255),
            Bengine::Justification::MIDDLE
        );
    }

    // Dialogue box message
    if (m_hasDialogueBox && (!m_fadeIn && !m_fadingOut)) {
        for (size_t i = 0; i < m_message.size(); i++) {
            sprintf_s(buffer, "%s", m_message[i].c_str());

            m_spriteFont.draw(
                m_spriteBatch,
                buffer,
                bottomLeft + glm::vec2(40, 127 - (i * (m_spriteFont.getFontHeight() / 1.5f))),
                glm::vec2(MESSAGE_SCALE),
                0.0f,
                Bengine::ColorRGBA8(0, 0, 0, 255)
            );
        }
    }

    // Options in question
    if (m_isQuestion && ((!m_fadeIn && !m_fadingOut) || m_waitAfterClickedOption)) {
        drawAnswerTexts();
    }
}

void MainGameScreen::drawAnswerBoxes(int redIdx /*= -1*/, int greenIdx /*= -1*/)
{
    static const Bengine::GLTexture blueBox = Bengine::ResourceManager::getTexture("Textures/Visuals/BlueOptionBox.png");
    static const Bengine::GLTexture greenBox = Bengine::ResourceManager::getTexture("Textures/Visuals/GreenOptionBox.png");
    static const Bengine::GLTexture redBox = Bengine::ResourceManager::getTexture("Textures/Visuals/RedOptionBox.png");
    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    int idx = 0;

    // Draw the boxes
    for (int y = ANSWER_BOX_SPACE; y >= -ANSWER_BOX_SPACE; y -= ANSWER_BOX_SPACE) {
        float x = -ANSWER_BOX_WIDTH / 2;
        
        Bengine::GLTexture texture = blueBox;
        
        if (redIdx == idx) texture = redBox;
        else if (greenIdx == idx) texture = greenBox;

        m_spriteBatch.draw(
            glm::vec4(x, y - ANSWER_BOX_HEIGHT / 2, ANSWER_BOX_WIDTH, ANSWER_BOX_HEIGHT),
            uvRect,
            texture.id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );

        if (m_answerBoxPositions.size() < 3) {
            // Get the locations of the boxes and push them to the position vector
            float xp = static_cast<float>((m_window->getScreenWidth() + x * 2) / 2);
            float yp = static_cast<float>(m_window->getScreenHeight() / 2 + y - ANSWER_BOX_HEIGHT / 2);
            m_answerBoxPositions.push_back(glm::vec2(xp, yp));
        }

        idx++;
    }
}

void MainGameScreen::drawAnswerTexts()
{
    char buffer[256];

    const float FONT_SCALE = 0.6f;
    std::vector<std::string> answers;
    answers.push_back(m_option1["text"].as<std::string>());
    answers.push_back(m_option2["text"].as<std::string>());
    answers.push_back(m_option3["text"].as<std::string>());
    static const Bengine::ColorRGBA8 color(0, 0, 0, 255);

    // Draw the answers inside the boxes
    unsigned i = 0;
    for (int y = ANSWER_BOX_SPACE; y >= -ANSWER_BOX_SPACE; y -= ANSWER_BOX_SPACE) {
        std::vector<std::string> wrappedText = getWrappedText(answers[i], ANSWER_BOX_WIDTH - 50, FONT_SCALE);

        // If the answer has more than 1 lines
        if (wrappedText.size() > 1) {
            for (unsigned j = 0; j < wrappedText.size(); j++) {
                sprintf_s(buffer, "%s", wrappedText[j].c_str());

                m_spriteFont.draw(
                    m_spriteBatch,
                    buffer,
                    glm::vec2(0.0f, y - j * ((m_spriteFont.getFontHeight() + 5) * FONT_SCALE)),
                    glm::vec2(FONT_SCALE),
                    0.0f,
                    color,
                    Bengine::Justification::MIDDLE
                );
            }
        }
        // If the answer only has 1 line
        else {
            sprintf_s(buffer, "%s", wrappedText[0].c_str());

            m_spriteFont.draw(
                m_spriteBatch,
                buffer,
                glm::vec2(0.0f, y - m_spriteFont.getFontHeight() * FONT_SCALE / 2),
                glm::vec2(FONT_SCALE),
                0.0f,
                color,
                Bengine::Justification::MIDDLE
            );
        }

        i++;
    }
}

void MainGameScreen::checkInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        m_game->onSDLEvent(event);

        switch (event.type) {
        case SDL_QUIT:
            SDL_Quit();
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
        }
    }

    if (m_game->inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
        if (!m_fadeIn && !m_fadingOut && !m_showOptions) {
            if (m_isQuestion) {
                handleOptionBoxInputs();
            }
            else {
                changeToNextDialogue();
            }
        }
    }

    if (m_game->inputManager.isKeyPressed(SDLK_o)) {
        if (m_showOptions) {
            m_showOptions = false;
        }
        else  {
            if (!m_fadingOut && !m_fadeIn) m_showOptions = true;
        }
    }
}

void MainGameScreen::initShaders()
{
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void MainGameScreen::initDay()
{
    m_file = YAML::LoadFile("Dialogues/" + m_currentDay + ".yaml");
    m_currentSceneIndex = 0;
    m_currentDialogueIndex = 0;
    m_currentSceneStr = "0";
    m_currentDialogueStr = "0";
    updateNewDialogueValues();
}

void MainGameScreen::changeToNextDialogue()
{
    // Don't change to next dialogue before faded out
    if (m_shouldFadeOut) {
        m_fadingOut = true;
        m_alphaValue = 255;
        return;
    }

    YAML::Node next = m_file[m_currentSceneStr][m_currentDialogueStr]["next"];
    YAML::Node nextScene = m_file[m_currentSceneStr][m_currentDialogueStr]["next_scene"];
    YAML::Node left = m_file[m_currentSceneStr][m_currentDialogueStr]["left"];
    YAML::Node right = m_file[m_currentSceneStr][m_currentDialogueStr]["right"];

    // Change to next dialogue in the same scene
    if (next != nullptr) {
        if (left != nullptr) {
            std::string l = left.as<std::string>();
            m_lastLeftChar = l.substr(0, l.find(","));

            m_lastLeftTexture = Bengine::ResourceManager::getTexture("Textures/Characters/" + m_lastLeftChar + "/" + l.substr(l.find(", ") + 2, l.length()));
        }
        if (right != nullptr) {
            std::string r = right.as<std::string>();
            m_lastRightChar = r.substr(0, r.find(","));

            m_lastRightTexture = Bengine::ResourceManager::getTexture("Textures/Characters/" + m_lastRightChar + "/" + r.substr(r.find(", ") + 2, r.length()));
        }

        m_currentDialogueStr = next.as<std::string>();
        m_currentDialogueIndex = stoi(m_currentDialogueStr);
    }
    // Change to next scene
    else if (nextScene != nullptr) {
        m_fadeLeftChar = false;
        m_fadeRightChar = false;

        m_currentSceneStr = nextScene.as<std::string>();
        m_currentSceneIndex = stoi(m_currentSceneStr);

        m_currentDialogueStr = "0";
        m_currentDialogueIndex = 0;
    }

    updateNewDialogueValues();
}

void MainGameScreen::updateNewDialogueValues()
{
    YAML::Node dlg = m_file[m_currentSceneStr][m_currentDialogueStr];
    YAML::Node left = m_file[m_currentSceneStr][m_currentDialogueStr]["left"];
    YAML::Node right = m_file[m_currentSceneStr][m_currentDialogueStr]["right"];
    YAML::Node talker = m_file[m_currentSceneStr][m_currentDialogueStr]["talker"];
    YAML::Node message = m_file[m_currentSceneStr][m_currentDialogueStr]["message"];
    YAML::Node fadeIn = m_file[m_currentSceneStr][m_currentDialogueStr]["FadeIn"];
    YAML::Node fadeOut = m_file[m_currentSceneStr][m_currentDialogueStr]["FadeOut"];
    YAML::Node startMusic = m_file[m_currentSceneStr][m_currentDialogueStr]["StartMusic"];
    YAML::Node endMusic = m_file[m_currentSceneStr][m_currentDialogueStr]["EndMusic"];
    YAML::Node soundEffect = m_file[m_currentSceneStr][m_currentDialogueStr]["SoundEffect"];

    // Left character
    if (left != nullptr) {
        std::string leftStr = left.as<std::string>();
        std::string name = leftStr.substr(0, leftStr.find(","));

        if (name == m_lastLeftChar && m_currentDialogueIndex != 0) {
            m_fadeLeftChar = true;
            m_leftCharAlpha = 0;
        }
        else {
            m_fadeLeftChar = false;
            m_leftCharAlpha = 255;
        }

        if (name == "Fumiko-san") {
            m_leftCharWidth = 220;
        }
        else if (name == "Hideo-kun" || name == "Teemu-kun") {
            m_leftCharWidth = 275;
        }
        else if (name == "Iwao-kun") {
            m_leftCharWidth = 242;
        }
        else if (name == "Maiko-san" || name == "Makoto-kun") {
            m_leftCharWidth = 225;
        }
        else if (name == "Michiko-san") {
            m_leftCharWidth = 237;
        }
    }
    else {
        m_leftCharWidth = -1;
    }

    // Right character
    if (right != nullptr) {
        std::string rightStr = right.as<std::string>();
        std::string name = rightStr.substr(0, rightStr.find(","));

        if (name == m_lastRightChar && m_currentDialogueIndex != 0) {
            m_fadeRightChar = true;
            m_rightCharAlpha = 0;
        }
        else {
            m_fadeRightChar = false;
            m_rightCharAlpha = 255;
        }

        if (name == "Fumiko-san") {
            m_rightCharWidth = 220;
        }
        else if (name == "Hideo-kun" || name == "Teemu-kun") {
            m_rightCharWidth = 275;
        }
        else if (name == "Iwao-kun") {
            m_rightCharWidth = 242;
        }
        else if (name == "Maiko-san" || name == "Makoto-kun") {
            m_rightCharWidth = 225;
        }
        else if (name == "Michiko-san") {
            m_rightCharWidth = 237;
        }
    }
    else {
        m_rightCharWidth = -1;
    }

    // Talker box
    if (talker != nullptr) {
        m_hasTalkerBox = true;

        m_talker = talker.as<std::string>();

        if (m_talker == "Player" || m_talker == "player") {
            m_talker = m_playerName;
        }
    }
    else {
        m_hasTalkerBox = false;
        m_talker = "";
    }

    // Dialogue box
    if (message != nullptr) {
        m_hasDialogueBox = true;

        std::string msg = message.as<std::string>();

        // Replace all "player's" with player's name
        std::string::size_type n = 0;
        while ((n = msg.find("player", n)) != std::string::npos) {
            msg.replace(n, 6, m_playerName);
            n += 6;
        }
        n = 0;
        while ((n = msg.find("Player", n)) != std::string::npos) {
            msg.replace(n, 6, m_playerName);
            n += 6;
        }

        m_message = getWrappedText(msg, 700.0f, MESSAGE_SCALE);
    }
    else {
        m_hasDialogueBox = false;
        m_message.clear();
    }

    // Question
    if (dlg["name"] != nullptr && dlg["name"].as<std::string>().find("Question") != std::string::npos) {
        m_isQuestion = true;

        m_option1 = dlg["Option 1"];
        m_option2 = dlg["Option 2"];
        m_option3 = dlg["Option 3"];
    }
    else {
        m_isQuestion = false;
    }

    // Fade in
    if (fadeIn != nullptr) {
        m_fadeIn = true;
        m_alphaValue = 0;
    }
    else {
        m_fadeIn = false;
    }

    // Fade out
    m_shouldFadeOut = (fadeOut != nullptr);

    // Start music
    if (startMusic != nullptr) {
        std::string filePath = "Audio/Music/" + startMusic.as<std::string>();
        m_currentSong = m_audioEngine.loadMusic(filePath);
        m_currentSong.play(-1);
    }

    // End music
    if (endMusic != nullptr) {
        m_currentSong.stop();
    }

    // Sound effect
    if (soundEffect != nullptr) {
        m_currentSoundEffect = m_audioEngine.loadSoundEffect("Audio/Sound Effects/" + soundEffect.as<std::string>());
        m_currentSoundEffect.play();
    }
}

void MainGameScreen::handleOptionBoxInputs()
{
    glm::vec2 mouseCoords = m_game->inputManager.getMouseCoords();

    int clickedIdx = -1;

    for (size_t i = 0; i < m_answerBoxPositions.size(); i++) {
        if (mouseCoords.x > m_answerBoxPositions[i].x && mouseCoords.x < m_answerBoxPositions[i].x + ANSWER_BOX_WIDTH) {
            if (mouseCoords.y > m_answerBoxPositions[i].y && mouseCoords.y < m_answerBoxPositions[i].y + ANSWER_BOX_HEIGHT) {
                clickedIdx = i;
                break;
            }
        }
    }

    // Reverse index
    if (clickedIdx != -1) {
        int diff = clickedIdx - m_answerBoxPositions.size();
        clickedIdx = abs(diff) - 1;

        // Don't change to next dialogue before faded out
        if (m_shouldFadeOut) {
            m_fadingOut = true;
            m_alphaValue = 255;
            m_clickedBoxIdx = clickedIdx;
        }
    }

    handleClickedOption(clickedIdx);
}

void MainGameScreen::handleClickedOption(int idx)
{
    // Get the index of the option that was clicked
    if (idx != -1) {
        // Put the current option into string to access it from the YAML file
        std::string optionString = "Option " + std::to_string(idx + 1);
        // Get the current option
        YAML::Node currNode = m_file[m_currentSceneStr][m_currentDialogueStr][optionString];

        int influence = currNode["influence"].as<int>();

        m_nextDlgFromOption = currNode["next"].as<int>();

        if (influence == 0) {
            m_clickedRedIdx = idx;
        }
        else if (influence == 2) {
            m_clickedGreenIdx = idx;
        }

        // Wait a bit after answering the question so the player sees how the answer influenced the character
        m_waitAfterClickedOption = true;
        m_firstUpdateAfterOptionClick = true;

        m_clickedBoxIdx = -1;

        // TODO: Get the name of the character who was asking the question
    }
}

std::vector<std::string> MainGameScreen::getWrappedText(std::string text, const float& maxLength, const float& fontScale)
{
    std::vector<std::string> lines;

    while (m_spriteFont.measure(text.c_str()).x * fontScale > maxLength) {
        // The current line that will be added
        std::string crop = "";
        for (unsigned i = 0; i < text.length(); i++) {
            crop += text[i];

            // If the line doesn't fit the box, it means that we should end the current line
            if (m_spriteFont.measure(crop.c_str()).x * fontScale > maxLength) {
                char currChar = text[i];

                // Go back to the last space so we don't crop in the middle of a word
                while (currChar != ' ') {
                    i--;
                    currChar = text[i];
                }
                crop = crop.substr(0, i);
                lines.push_back(crop);
                text = text.substr(i + 1, text.length() - 1);
                break;
            }
        }
    }

    if (text != "") lines.push_back(text);

    return lines;
}

void MainGameScreen::loadOptions()
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

    m_audioEngine.setVolume(volume);
}

void MainGameScreen::saveOptions()
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