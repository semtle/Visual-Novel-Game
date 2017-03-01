#pragma once

#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <Bengine/AudioEngine.h>

#include <yaml-cpp/yaml.h>

class MainGameScreen : public Bengine::IGameScreen
{
public:
    MainGameScreen(Bengine::Window* window);
    ~MainGameScreen();


    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    virtual void build() override;
    virtual void destroy() override;
    virtual void onEntry() override;
    virtual void onExit() override;
    virtual void update() override;
    virtual void draw() override;

    void setPlayerName(const std::string playerName) { m_playerName = playerName; }

private:
    void drawImages();
    void drawAnswerBoxes(int redIdx = -1, int greenIdx = -1);
    void drawTexts();
    void drawAnswerTexts();
    void checkInput();
    void initShaders();
    void initDay();
    void changeToNextDialogue();
    void updateNewDialogueValues();
    void handleOptionBoxInputs();
    void handleClickedOption(int idx);
    std::vector<std::string> getWrappedText(std::string text, const float& maxLength, const float& fontScale);

    std::string m_currentDay = "Monday";
    std::string m_currentSceneStr = "-1", m_currentDialogueStr = "-1";
    int m_currentSceneIndex = -1;
    int m_currentDialogueIndex = -1;
    int m_nextDlgFromOption = -1;

    int m_clickedRedIdx = -1;
    int m_clickedGreenIdx = -1;
    int m_clickedBoxIdx = -1;

    int m_leftCharWidth = -1;
    int m_rightCharWidth = -1;
    int m_alphaValue = 255;

    bool m_hasDialogueBox = false;
    bool m_hasTalkerBox = false;
    bool m_isQuestion = false;
    bool m_fadeIn = false;
    bool m_changeToNextDialogue = true;
    bool m_shouldFadeOut = false;
    bool m_fadingOut = false;
    bool m_firstUpdateAfterOptionClick = false;
    bool m_waitAfterClickedOption = false;

    std::string m_playerName = "";
    std::string m_talker = "";
    std::vector<std::string> m_message;
    std::vector<glm::vec2> m_answerBoxPositions;

    YAML::Node m_option1, m_option2, m_option3;

    YAML::Node m_file;

    Bengine::SoundEffect m_currentSoundEffect;
    Bengine::Music m_currentSong;
    Bengine::Window* m_window;
    Bengine::AudioEngine m_audioEngine;
    Bengine::Camera2D m_camera;
    Bengine::SpriteBatch m_spriteBatch;
    Bengine::SpriteFont m_spriteFont;
    Bengine::GLSLProgram m_textureProgram;
};

