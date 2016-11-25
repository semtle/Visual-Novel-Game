#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

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

	void saveScenes(const std::string& filePath);

	std::map<int, std::string> getScenes() const { return this->scenes; }
private:
	std::map<int, std::string> scenes;
};