#pragma once

#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <vector>
#include <glm/glm.hpp>

struct SaveData {
    int sceneIndex;
    std::string playerName;
    std::string sceneName;
    Bengine::GLTexture background;
};

class LoadGameScreen
{
public:
    LoadGameScreen();
    ~LoadGameScreen();

    void init(Bengine::Window* window);
    void update();
    void draw(Bengine::SpriteBatch* spriteBatch, Bengine::SpriteFont* spriteFont);
    void onMouseDown(float x, float y);
    void onMouseUp();
    void onMouseMove(float x, float y);

    void open() { m_shouldClose = false; }

    void setShouldClose(bool s) { m_shouldClose = s; }

    bool shouldClose() const { return m_shouldClose; }
    bool loaded()      const { return m_loaded; }
    SaveData getSave()       { return m_loadedSave; m_loaded = false; }

private:
    std::vector<std::string> getSaveFiles();
    glm::vec4 getInputDimensions(glm::vec4 destRect);

    std::vector<std::string> m_saveFilePaths;
    std::vector<SaveData> m_saveDatas;

    std::vector<glm::vec2> m_saveSlotPositions;

    glm::vec4 m_backButtonDestRect, m_loadButtonDestRect;

    bool m_shouldClose = false;
    bool m_loaded = false;

    SaveData m_loadedSave;

    int m_selectedSlotIdx = -1;

    Bengine::Window* m_window;
};