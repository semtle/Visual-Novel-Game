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

enum class ProgramState { FILESELECT, MAINSCREEN, ADDSCENE, EXIT };

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

	void checkFileSelectInputs();
	void checkMainScreenInputs();
	void checkSceneCreationScreenInputs();
	void onKeyPress(unsigned int keyID);

	void drawFileSelectScreen();
	void drawMainScreen();
	void drawSceneCreationScreen();

	void drawFileSelectTexts();
	void drawMainScreenTexts();
	void drawSceneCreationScreenTexts();
private:
	std::map<int, std::pair<std::string, Dialogue>> getShownScenes(std::map<int, std::pair<std::string, Dialogue>> allScenes);
	glm::vec4 getInputDimensions(glm::vec4 texture);

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
	int screenHeight = 600;

	std::string currentSceneName = "";

	static const int BUTTONS_MIDDLE_HORIZONTAL_RADIUS = 60;
	static const int BUTTONS_MIDDLE_VERTICAL_RADIUS = -150;
	static const int BUTTON_WIDTH = 149;
	static const int BUTTON_HEIGHT = 38;

	// Main window bg
	glm::vec4 mainBgDestRect = glm::vec4(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);
	// Cancel button
	glm::vec4 cancelBtnDestRect = glm::vec4(this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS, this->BUTTONS_MIDDLE_VERTICAL_RADIUS, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);
	// Create button
	glm::vec4 createBtnDestRect = glm::vec4(-this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS - 149, this->BUTTONS_MIDDLE_VERTICAL_RADIUS, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);

	// 'Add new' -button
	glm::vec4 addNewBtnDestRect = glm::vec4(-this->screenWidth / 2 + 50, -this->screenHeight / 2 + 30, 149, 38);

	// Main UV rect for most textures
	glm::vec4 mainUvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// Main color
	Bengine::ColorRGBA8 color = Bengine::ColorRGBA8(255, 255, 255, 255);

	// Arrows
	glm::vec4 upArrowDestDect = glm::vec4(-this->screenWidth / 2 + 52, this->screenHeight / 2 - 150, 86, 43);
	glm::vec4 downArrowDestDect = glm::vec4(-this->screenWidth / 2 + 52, -this->screenHeight / 2 + 60, 86, 43);

	// Background for scene creation window
	glm::vec4 createSceneBgDestRect = glm::vec4(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);

	// 'Add New' -button
	glm::vec4 addNewDestRect = glm::vec4(-this->screenWidth / 2 + 31, -this->screenHeight / 2 + 20, 134, 34);

	// Box for scene information
	glm::vec4 sceneBoxDestRect = glm::vec4(-this->screenWidth / 2 + 7, this->screenHeight / 2 - 234, 185, 85);

	// UV-Rect for up-arrow (rotate arrow 180 degrees)
	glm::vec4 upArrowUvRect = glm::vec4(0.0f, 0.0f, 1.0f, -1.0f);

	int currentSceneListIdx = 0;

	ProgramState currentState = ProgramState::MAINSCREEN;
};