#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>

#include <iostream>
#include <string>
#include <vector>

#include <Bengine/Bengine.h>
#include <Bengine/Window.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteFont.h>

class SceneManager
{
public:
	SceneManager(Bengine::InputManager* InputManager);
	~SceneManager();

	void update();
private:
	void processInput();
	void createSceneWindow();
	void handleSceneWindow();
	void processSceneWindowInput();
	void drawSceneWindow();

	Bengine::InputManager* inputManager;

	Bengine::Window sceneWindow;

	Bengine::Camera2D sceneCamera;

	Bengine::SpriteBatch spriteBatch;

	Bengine::GLSLProgram shaderProgram;

	int sceneWindowWidth = 400;
	int sceneWindowHeight = 400;
	unsigned int cancelButton = Bengine::ResourceManager::getTexture("Textures/cancel-button.png").id;
	unsigned int createButton = Bengine::ResourceManager::getTexture("Textures/create-button.png").id;
	bool isSceneWindowOpen = false;
};