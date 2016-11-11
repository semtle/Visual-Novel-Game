#pragma once
#include "Scene.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <Bengine/ResourceManager.h>
#include <Bengine/InputManager.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/BengineErrors.h>

#include "yaml-cpp/yaml.h"
#include "Dialogue.h"
#include "Character.h"

class Day : public Scene
{
public:
	Day();
	~Day();

	void init(Bengine::InputManager* manager, const std::map<std::string, Character *>& characters, const int& screenWidth, const int& screenHeight) override;
	void update() override;
	void processInputs();
	void doFading() override;
	bool exitGame() override;
	void setDay(std::string dayName);
	void setPlayerName(std::string playerName);
	std::string changeScene() override;

	void drawTexts(
		Bengine::SpriteFont* spriteFont,
		Bengine::GLSLProgram* shaderProgram,
		const int& screenWidth,
		const int& screenHeight
	) override;

	void drawImages(
		Bengine::SpriteBatch& spriteBatch,
		Bengine::Camera2D* hudCamera,
		Bengine::GLSLProgram* shaderProgram,
		const int& screenWidth,
		const int& screenHeight
	) override;

	void waitAfterQuestion();
private:
	void nextDialogue();

	unsigned int currentDialogueID;
	std::string playerName;
	std::string day;
	YAML::Node file;
	int greenBoxIdx = -1;
	int redBoxIdx = -1;
	int currentDialogueIdx = 0;
	int currentSceneIdx = 0;
	bool wait = false;
	std::string optionString;
	bool inQuestion = false;
	std::string currentScene;
	std::string currentDialogue;

	std::vector<std::string> answers;
	std::vector<Dialogue *> dialogues;
	std::map<std::string, Character *> characters;

	std::vector<std::string> dialogueNames;
	std::vector<std::string> sceneNames;
};