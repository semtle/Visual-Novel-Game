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

struct Dialogue {
	Dialogue() :
		index(-1),
		name(""),
		background(""),
		talking(""),
		left(""),
		right(""),
		message(""),
		nextDialogue(-1)
	{ }

	Dialogue(
		int Index,
		std::string Name,
		std::string Background,
		std::string Talking,
		std::string Left,
		std::string Right,
		std::string Message,
		int NextDialogue
	) :
		index(Index),
		name(Name),
		background(Background),
		talking(Talking),
		left(Left),
		right(Right),
		message(Message),
		nextDialogue(NextDialogue)
	{ }

	int index;
	std::string name;
	std::string background;
	std::string talking;
	std::string left;
	std::string right;
	std::string message;
	int nextDialogue;
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void update();

	void addScene(std::string sceneName);
	void addDialogue(int idx, Dialogue *);

	void saveScenes(const std::string& filePath);

	// Getters
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> getScenes() const { return this->scenes; }
	std::vector<Dialogue *> getDialogues(int idx) { return this->scenes[idx].second; }

	
private:
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> scenes;
};