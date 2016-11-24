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

enum class ProgramState { MAINSCREEN, ADDSCENE, EXIT };

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

	void checkMainScreenInputs();
	void checkSceneCreationScreenInputs();
	void onKeyPress(unsigned int keyID);

	void drawMainScreen();
	void drawSceneCreationScreen();

	void drawMainScreenTexts();
	void drawSceneCreationScreenTexts();
private:
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

	ProgramState currentState = ProgramState::MAINSCREEN;
};