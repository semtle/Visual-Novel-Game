#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <Bengine/SpriteBatch.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/SpriteFont.h>

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void init(Bengine::InputManager* manager, const int& screenWidth, const int& screenHeight) = 0;
	virtual void update() = 0;
	virtual bool exitGame() = 0;
	virtual std::string changeScene() = 0;
	virtual void drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight) = 0;
	virtual void drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight) = 0;
protected:
	Bengine::SpriteBatch fontBatch;
	std::string nextScene = "";
	Bengine::Camera2D camera;
	Bengine::InputManager* inputManager;
};