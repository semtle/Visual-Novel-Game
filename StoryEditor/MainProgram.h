#pragma once

#include "SceneManager.h"

#include <iostream>
#include <string>

#include <Bengine/Bengine.h>
#include <Bengine/Window.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteFont.h>

enum class ProgramState { FILESELECT, MAINSCREEN, ADDSCENE, ADD_DIALOGUE, EXIT };

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
	void update();

	void checkFileSelectInputs();
	void checkMainScreenInputs();
	void checkSceneCreationScreenInputs();
	void onKeyPress(unsigned int keyID);

	void drawFileSelectScreen();
	void drawMainScreen();
	void drawCurrentDialogue();

	void drawSceneCreationScreen();

	void drawFileSelectTexts();
	void drawMainScreenTexts();
	void drawSceneCreationScreenTexts();
private:
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> getShownScenes(std::map<int, std::pair<std::string, std::vector<Dialogue *>>> allScenes);
	glm::vec4 getInputDimensions(glm::vec4 texture, bool swapy = true);

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

	std::string currentSceneName = "";
	std::string currentDialogueName = "";

	Dialogue currentDialogue;

	std::vector<std::pair<int, glm::vec2>> shownSceneBlockPositions;

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

	glm::vec4 blackBoxDestRect = glm::vec4(-this->screenWidth / 2 + 200, -this->screenHeight / 2 + 65, 600, 450);

	// 'Add new' -button
	glm::vec4 addNewBtnDestRect = glm::vec4(-this->screenWidth / 2 + 50, -this->screenHeight / 2 + 30, 149, 38);

	// Main UV rect for most textures
	glm::vec4 mainUvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// Main color
	Bengine::ColorRGBA8 color = Bengine::ColorRGBA8(255, 255, 255, 255);

	// Arrows
	glm::vec4 upArrowDestDect = glm::vec4(-this->screenWidth / 2 + 52, this->screenHeight / 2 - 155, 86, 43);
	glm::vec4 downArrowDestDect = glm::vec4(-this->screenWidth / 2 + 52, -this->screenHeight / 2 + 60, 86, 43);

	// Background for scene creation window
	glm::vec4 createSceneBgDestRect = glm::vec4(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);

	// 'Add New' -button
	glm::vec4 addNewDestRect = glm::vec4(-this->screenWidth / 2 + 31, -this->screenHeight / 2 + 20, 134, 34);

	// Box for scene information
	glm::vec4 sceneBoxDestRect = glm::vec4(sceneBlockDrawStartPos, SCENE_BOX_WIDTH, SCENE_BOX_HEIGHT);

	// Background button
	glm::vec4 bgBtnDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 540, 149, 37);

	// Char 1 button
	glm::vec4 char1BtnDestRect = glm::vec4(-this->screenWidth / 2 + 220, -this->screenHeight / 2 + 20, buttonSize);

	// Save button
	glm::vec4 saveBtnDestRect = glm::vec4(-this->screenWidth / 2 + 400, -this->screenHeight / 2 + 20, buttonSize);

	// Delete button
	glm::vec4 deleteBtnDestRect = glm::vec4(-this->screenWidth / 2 + 520, -this->screenHeight / 2 + 20, buttonSize);

	// Char 2 button
	glm::vec4 char2BtnDestRect = glm::vec4(-this->screenWidth / 2 + 680, -this->screenHeight / 2 + 20, buttonSize);

	// Left arrow
	glm::vec4 arrowLeftDestRect = glm::vec4(-this->screenWidth / 2 + 135, -this->screenHeight / 2 + 605, 49, 30);

	// UV-Rect for up-arrow (rotate arrow 180 degrees)
	glm::vec4 upArrowUvRect = glm::vec4(0.0f, 0.0f, 1.0f, -1.0f);

	int currentSceneListIdx = 0;
	int selectedSceneIdx = -1;
	int selectedDialogueIdx = -1;

	ProgramState currentState = ProgramState::MAINSCREEN;
};