#pragma once

#include "SceneManager.h"

#include <iostream>
#include <string>
#include <algorithm>

#include <direct.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>

#include <windows.h>
#include <Commdlg.h>

#include <Bengine/Bengine.h>
#include <Bengine/Window.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteFont.h>

enum class ProgramState { FILESELECT, MAINSCREEN, ADDSCENE, ADD_DIALOGUE, DELETE_DIALOGUE, EXIT };

class MainProgram
{
public:
	MainProgram();
	~MainProgram();

	void run();
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
	void drawScreen();
	void updateBoxes();

	void checkFileSelectInputs();
	void checkMainScreenInputs();
	void checkSceneCreationScreenInputs();
	void checkDialogueDeletionInputs();
	void onKeyPress(unsigned int keyID);

	void drawFileSelectScreen();
	void drawMainScreen();
	void drawCurrentDialogue();
	void drawDialogueDeletionScreen();

	void submitDialogue();

	void drawSceneCreationScreen();

	void drawFileSelectTexts();
	void drawMainScreenTexts();
	void drawSceneCreationScreenTexts();
	void drawDialogueDeletionTexts();

	void onScroll(int yDir);
private:
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> getShownScenes(std::map<int, std::pair<std::string, std::vector<Dialogue *>>> allScenes);
	std::vector<Dialogue *> getShownDialogues(std::vector<Dialogue *> allDialogues);
	std::vector<std::string> getWrappedText(std::string text, Bengine::SpriteFont* spriteFont, const float& maxLength, const float& fontScale);
	glm::vec4 getInputDimensions(glm::vec4 texture, bool swapy = true);
	std::string getBoxSizeText(std::string text, float scale);
    std::wstring getOpenFileName(std::string fileType);
	void resetEverything();

	void resetCurrentDialogue();

	ProgramState currentState = ProgramState::FILESELECT;

	Bengine::Window window; ///< Main window

	Bengine::GLSLProgram shaderProgram; ///< Shaders

	Bengine::SpriteBatch spriteBatch; ///< Main sprite batch
	Bengine::SpriteBatch fontBatch; ///< Batch for fonts

	Bengine::InputManager inputManager; ///< Main input manager

	Bengine::Camera2D camera; ///< Main camera
	Bengine::Camera2D fontCamera; ///< Camera for texts

	Bengine::SpriteFont* spriteFont; ///< Main font

	SceneManager* sceneManager; ///< Scene manager

	int screenWidth = 800;
	int screenHeight = 710;

	glm::vec2 sceneBlockDrawStartPos = glm::vec2(-screenWidth / 2 + 7, -screenHeight / 2 + 469);
	int sceneBlockPositionDiff = 90;

	std::vector<int> shownSceneIndexes;
	std::vector<int> shownDialogueIndexes;

	std::string currentSceneName = "";
	std::string currentDialogueName = "";
	std::string currentFileName = "";
    std::string creationErrorMsg = "";

	bool clickedOnTalkerBox = false;
	bool clickedOnDialogueBox = false;

	bool clickedOnFirstAnswerBox = false;
	bool clickedOnSecondAnswerBox = false;
	bool clickedOnThirdAnswerBox = false;

	bool changingSettings = false;
	bool settingNextDialogue = false;
	bool customNextDlg = false;
	bool changingDialogueName = false;

	bool duplicateSceneName = false;
	bool duplicateDialogueName = false;

	bool deletingScene = false;

	Dialogue* currentDialogue;
	Dialogue* lastDialogue;

	std::vector<std::pair<int, glm::vec2>> shownSceneBlockPositions;
	std::vector<std::pair<int, glm::vec2>> showDialogueBoxPositions;

	static const int BUTTONS_MIDDLE_HORIZONTAL_RADIUS = 60;
	static const int BUTTONS_MIDDLE_VERTICAL_RADIUS = -150;
	static const int BUTTON_WIDTH = 149;
	static const int BUTTON_HEIGHT = 38;
	static const int SCENE_BOX_WIDTH = 185;
	static const int SCENE_BOX_HEIGHT = 85;

	glm::vec2 buttonSize = glm::vec2(99, 37);

	// Main window bg
	glm::vec4 mainBgDestRect = glm::vec4(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);
	// Cancel button
	glm::vec4 cancelBtnDestRect = glm::vec4(this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS, this->BUTTONS_MIDDLE_VERTICAL_RADIUS, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);
	// Create button
	glm::vec4 createBtnDestRect = glm::vec4(-this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS - 149, this->BUTTONS_MIDDLE_VERTICAL_RADIUS, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);

	// BG of the current dialog
	glm::vec4 dialogueBgDestRect = glm::vec4(-this->screenWidth / 2 + 200, -this->screenHeight / 2 + 73, 600, 450);

	// Left character
	glm::vec4 leftCharDestRect = glm::vec4(-this->screenWidth / 2 + 255, -this->screenHeight / 2 + 73, 165, 440);

	// Left character
	glm::vec4 rightCharDestRect = glm::vec4(-this->screenWidth / 2 + 565, -this->screenHeight / 2 + 73, 165, 440);

	// 'Add new' -button
	glm::vec4 addNewBtnDestRect = glm::vec4(-this->screenWidth / 2 + 50, -this->screenHeight / 2 + 30, 149, 38);

	// 'Open File' -button
	glm::vec4 openFileBtnDestRect = glm::vec4(-150 - this->buttonSize.x / 2, -100, this->buttonSize);

	// 'Edit File' -button
	glm::vec4 editFileBtnDestRect = glm::vec4(-this->buttonSize.x / 2, -100, this->buttonSize);

	// 'Exit' -button
	glm::vec4 exitBtnDestRect = glm::vec4(150 - this->buttonSize.x / 2, -100, this->buttonSize);

	// Main UV rect for most textures
	glm::vec4 mainUvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// Flipped x uv rect
	glm::vec4 flippedXUvRect = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

	// Main color
	Bengine::ColorRGBA8 color = Bengine::ColorRGBA8(255, 255, 255, 255);

	// Arrows
	glm::vec4 upArrowDestDect = glm::vec4(-this->screenWidth / 2 + 52, this->screenHeight / 2 - 155, 86, 43);
	glm::vec4 downArrowDestDect = glm::vec4(-this->screenWidth / 2 + 52, -this->screenHeight / 2 + 60, 86, 43);

	// Dialogue box inside the dialogues
	glm::vec4 textBoxDestRect = glm::vec4(-this->screenWidth / 2 + 200, -this->screenHeight / 2 + 73, 600, 160);

	// Talker box
	glm::vec4 talkerBoxDestRect = glm::vec4(-this->screenWidth / 2 + 215, textBoxDestRect.y + textBoxDestRect.a - 13, 123, 45);

	// Background for scene creation window
	glm::vec4 createSceneBgDestRect = glm::vec4(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);

	// 'Add New' -button
	glm::vec4 addNewDestRect = glm::vec4(-this->screenWidth / 2 + 31, -this->screenHeight / 2 + 20, 134, 34);

	// 'Cancel Next Dialogue'
	glm::vec4 cancelNextDlgBtnDestRect = glm::vec4(this->screenWidth / 2 - 200, -this->screenHeight / 2 + 20, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);

	// Box for scene information
	glm::vec4 sceneBoxDestRect = glm::vec4(sceneBlockDrawStartPos, SCENE_BOX_WIDTH, SCENE_BOX_HEIGHT);

	// Background button
	glm::vec4 bgBtnDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 540, 149, 37);

	// 'Set Next Dialogue' button
	glm::vec4 setNextBtnDestRect = glm::vec4(-this->screenWidth / 2 + 248, -this->screenHeight / 2 + 20, 149, 37);

	// Char 1 button
	glm::vec4 char1BtnDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 20, buttonSize);

	// Save button
	glm::vec4 saveBtnDestRect = glm::vec4(-this->screenWidth / 2 + 335, -this->screenHeight / 2 + 20, buttonSize);

	// Settings button
	glm::vec4 settingsBtnDestRect = glm::vec4(-this->screenWidth / 2 + 450, -this->screenHeight / 2 + 20, buttonSize);

	// Delete dialogue button
	glm::vec4 deleteBtnDestRect = glm::vec4(-this->screenWidth / 2 + 565, -this->screenHeight / 2 + 20, buttonSize);

	// Confirm dialogue delete
	glm::vec4 confirmDlgDeleteBtnDestRect = glm::vec4(-100 - buttonSize.x / 2, -100, buttonSize);

	// Cancel dialogue delete
	glm::vec4 cancelDlgDeleteBtnDestRect = glm::vec4(100 - buttonSize.x / 2, -100, buttonSize);

	// Delete scene button
	glm::vec4 deleteSceneBtnDestRect = glm::vec4(-this->screenWidth / 2 + 207, -this->screenHeight / 2 + 355, buttonSize);

	// Cancel scene delete
	glm::vec4 cancelSceneDeleteBtnDestRect = glm::vec4(-this->screenWidth / 2 + 327, -this->screenHeight / 2 + 355, buttonSize);

	// Char 2 button
	glm::vec4 char2BtnDestRect = glm::vec4(-this->screenWidth / 2 + 680, -this->screenHeight / 2 + 20, buttonSize);

	// First answer box
	glm::vec4 firstAnswerBoxDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 389, 560, 86);

	// Second answer box
	glm::vec4 secondAnswerBoxDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 255, 560, 86);

	// Third answer box
	glm::vec4 thirdAnswerBoxDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 121, 560, 86);

	// Show dialogue box checkbox
	glm::vec4 showDlgCheckBox = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 520, 32, 32);

	// Talker checkbox
	glm::vec4 talkerCheckBox = glm::vec4(-this->screenWidth / 2 + 480, -this->screenHeight / 2 + 520, 32, 32);

    // Fade in checkbox
    glm::vec4 fadeInCheckBox = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 440, 32, 32);

    // Fade out checkbox
    glm::vec4 fadeOutCheckBox = glm::vec4(-this->screenWidth / 2 + 350, -this->screenHeight / 2 + 440, 32, 32);

	// Question checkbox
	glm::vec4 questionCheckBox = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 360, 32, 32);

	// Custom next dlg checkbox
	glm::vec4 customNextCheckBox = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 280, 32, 32);

	// Custom next dialogue button
	glm::vec4 setNextCustomDlgDestRect = glm::vec4(-this->screenWidth / 2 + 225, -this->screenHeight / 2 + 220, 149, 37);

    // Start music checkbox
    glm::vec4 startMusicCheckBox = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 125, 32, 32);

    // End music checkbox
    glm::vec4 endMusicCheckBox = glm::vec4(-this->screenWidth / 2 + 420, -this->screenHeight / 2 + 125, 32, 32);

    // Sound effect checkbox
    glm::vec4 soundEffectCheckBox = glm::vec4(-this->screenWidth / 2 + 620, -this->screenHeight / 2 + 125, 32, 32);

    // Open music file
    glm::vec4 openMusicFileBtn = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 75, buttonSize);

    // Open sound effect file
    glm::vec4 openSoundEffectFileBtn = glm::vec4(-this->screenWidth / 2 + 620, -this->screenHeight / 2 + 75, buttonSize);

	// Close icon
	glm::vec4 closeIconDestRect = glm::vec4(this->screenWidth / 2 - 48, -this->screenHeight / 2 + 550, 32, 32);

	// Left arrow
	glm::vec4 arrowLeftDestRect = glm::vec4(-this->screenWidth / 2 + 135, -this->screenHeight / 2 + 615, 49, 30);

	// UV-Rect for up-arrow (rotate arrow 180 degrees)
	glm::vec4 upArrowUvRect = glm::vec4(0.0f, 0.0f, 1.0f, -1.0f);

	unsigned currentSceneListIdx = 0;
	unsigned currentDialogueListIdx = 0;

	int selectedAnswerBox = -1;
	int selectedNextSceneIdx = -1;

	int changingSceneNameIdx = -1;
	int selectedSceneIdx = -1;
	int selectedDialogueIdx = -1;

	int lastSceneIdx = -1;
	int lastDialogueIdx = -1;
	int lastSceneListIdx = -1;
	int lastDialogueListIdx = -1;

	bool dlgButtonsOnRight = false;
	bool saved = false;
};