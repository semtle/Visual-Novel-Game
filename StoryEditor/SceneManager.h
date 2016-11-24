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
	SceneManager();
	~SceneManager();

	void update();

	void addScene(std::string sceneName);

	std::vector<std::string> getScenes() const { return this->scenes; }
private:
	std::vector<std::string> scenes;
};