#include "LoadGameScreen.h"
#include <fstream>
#include <Bengine/IOManager.h>

const glm::vec2 SAVESLOT_DIMS(192, 229);

LoadGameScreen::LoadGameScreen()
{
}


LoadGameScreen::~LoadGameScreen()
{
}

void LoadGameScreen::init(Bengine::Window* window)
{
    m_window = window;
    m_saveFilePaths = getSaveFiles();

    m_backButtonDestRect = glm::vec4(-154 / 2, -m_window->getScreenHeight() / 2 + 25, 154, 57);
    m_loadButtonDestRect = glm::vec4(-154 / 2, -m_window->getScreenHeight() / 2 + 90, 154, 57);

    glm::vec2 bottomLeft(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2);
    m_saveSlotPositions.push_back(glm::vec2(bottomLeft.x + 30, bottomLeft.y + 320));
    m_saveSlotPositions.push_back(glm::vec2(bottomLeft.x + m_window->getScreenWidth() - 30 - SAVESLOT_DIMS.x, bottomLeft.y + 320));
    m_saveSlotPositions.push_back(glm::vec2(bottomLeft.x + 30, bottomLeft.y + 80));
    m_saveSlotPositions.push_back(glm::vec2(bottomLeft.x + m_window->getScreenWidth() - 30 - SAVESLOT_DIMS.x, bottomLeft.y + 80));
}

void LoadGameScreen::update()
{

}

void LoadGameScreen::draw(Bengine::SpriteBatch* spriteBatch, Bengine::SpriteFont* spriteFont)
{
    static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    { // Background
        glm::vec4 destRect(-m_window->getScreenWidth() / 2, -m_window->getScreenHeight() / 2, m_window->getScreenWidth(), m_window->getScreenHeight());

        spriteBatch->draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Backgrounds/Main Menu/LoadGameBG.png").id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );
    }

    { // Load Game text
        glm::vec4 destRect(-336 / 2, m_window->getScreenHeight() / 2 - 76, 336, 61);

        spriteBatch->draw(
            destRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/LoadGameText.png").id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );
    }

    // Save slots
    for (size_t i = 0; i < m_saveSlotPositions.size(); i++) {
        Bengine::GLTexture texture;
        if (m_selectedSlotIdx != i) texture = Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/SaveSlot.png");
        else texture = Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/SaveSlotHighlight.png");

        spriteBatch->draw(
            glm::vec4(m_saveSlotPositions[i].x, m_saveSlotPositions[i].y, SAVESLOT_DIMS),
            uvRect,
            texture.id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );

        // Just draw empty slot to every slot, if it's not empty background will cover it
        spriteFont->draw(
            *spriteBatch,
            "Empty Slot",
            glm::vec2(m_saveSlotPositions[i].x + SAVESLOT_DIMS.x / 2, m_saveSlotPositions[i].y + SAVESLOT_DIMS.y / 2 - spriteFont->measure("Empty Slot").y * 0.7f / 2 + 5),
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255),
            Bengine::Justification::MIDDLE
        );
    }

    char buffer[256];

    for (size_t i = 0; i < m_saveFilePaths.size(); i++) {
        sprintf_s(buffer, "%s", m_saveFilePaths[i].c_str());

        spriteFont->draw(
            *spriteBatch,
            buffer,
            glm::vec2(m_saveSlotPositions[i].x + SAVESLOT_DIMS.x / 2 - 5, m_saveSlotPositions[i].y + SAVESLOT_DIMS.y - 39),
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255),
            Bengine::Justification::MIDDLE
        );

        sprintf_s(buffer, "%s", m_saveDatas[i].sceneName.c_str());

        spriteFont->draw(
            *spriteBatch,
            buffer,
            glm::vec2(m_saveSlotPositions[i].x + SAVESLOT_DIMS.x / 2 - 3, m_saveSlotPositions[i].y + 8),
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255),
            Bengine::Justification::MIDDLE
        );

        // Background
        spriteBatch->draw(
            glm::vec4(m_saveSlotPositions[i].x + 7, m_saveSlotPositions[i].y + 46, 178, 134),
            uvRect,
            m_saveDatas[i].background.id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );
    }

    { // Back button
        spriteBatch->draw(
            m_backButtonDestRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/LightBackButton.png").id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );
    }

    if (m_selectedSlotIdx != -1) { // Load button
        spriteBatch->draw(
            m_loadButtonDestRect,
            uvRect,
            Bengine::ResourceManager::getTexture("Textures/Visuals/Main Menu/LoadButton.png").id,
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255)
        );
    }
}

void LoadGameScreen::onMouseDown(float x, float y)
{
    glm::vec4 dims = getInputDimensions(m_backButtonDestRect);

    // Back button
    if (x > dims.x && x < dims.x + dims.z) {
        if (y > dims.y && y < dims.y + dims.a) {
            m_shouldClose = true;
        }
    }

    dims = getInputDimensions(m_loadButtonDestRect);

    // Load button
    if (x > dims.x && x < dims.x + dims.z) {
        if (y > dims.y && y < dims.y + dims.a) {
            if (m_selectedSlotIdx != -1) {
                m_loadedSave = m_saveDatas[m_selectedSlotIdx];
                m_loaded = true;
                m_shouldClose = true;
            }
        }
    }

    m_selectedSlotIdx = -1;

    // Check if clicked on save slots
    for (size_t i = 0; i < m_saveFilePaths.size(); i++) {
        glm::vec4 dims = getInputDimensions(glm::vec4(m_saveSlotPositions[i], SAVESLOT_DIMS));

        if (x > dims.x && x < dims.x + dims.z) {
            if (y > dims.y && y < dims.y + dims.a) {
                m_selectedSlotIdx = i;
                break;
            }
        }
    }
}

void LoadGameScreen::onMouseUp()
{

}

void LoadGameScreen::onMouseMove(float x, float y)
{

}

std::vector<std::string> LoadGameScreen::getSaveFiles()
{
    std::vector<Bengine::DirEntry> files;
    Bengine::IOManager::getDirectoryEntries("SaveFiles/", files);
    std::vector<std::string> saveFiles;

    for (auto& f : files) {
        if (!f.isDirectory && f.path.find("options.txt") == std::string::npos) {
            if (saveFiles.size() == 4) {
                puts("You have some additional files in your saves folder. For your own safety remove them or they might mess up your saves.");
                break;
            }

            std::string end = f.path.substr(f.path.find("SaveFiles\\") + 10, f.path.length());
            end = end.substr(0, end.find(".savefile"));

            saveFiles.push_back(end);
        }
    }

    for (auto& f : saveFiles) {
        std::ifstream file("SaveFiles/" + f + ".savefile");

        if (!file.is_open()) {
            perror(f.c_str());
            SDL_Quit();
        }

        std::string line;

        int sceneIdx;
        std::string playerName;
        std::string sceneName;
        std::string texturePath;
        Bengine::GLTexture bg;

        std::vector<std::string> lines;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        // Get data from file
        sceneIdx = stoi(lines[0]);
        playerName = lines[1];
        sceneName = lines[2];
        texturePath = lines[3];

        std::cout << texturePath << "\n";

        // Load image
        bg = Bengine::ResourceManager::getTexture(texturePath);

        SaveData save;
        save.background = bg;
        save.playerName = playerName;
        save.sceneName = sceneName;
        save.sceneIndex = sceneIdx;

        // Add data to save datas
        m_saveDatas.push_back(save);

        file.close();
    }

    return saveFiles;
}

glm::vec4 LoadGameScreen::getInputDimensions(glm::vec4 destRect)
{
    destRect.x = destRect.x + m_window->getScreenWidth() / 2;

    // Flips the y value
    destRect.y = abs((destRect.y + m_window->getScreenHeight() / 2) - m_window->getScreenHeight()) - destRect.a;

    return destRect;
}