#pragma once

#include "Scene.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <Bengine/Vertex.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/Camera2D.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>

#include "Character.h"

enum class MenuState {MENU, NEWGAME, LOADGAME, QUIT};

class MainMenu : public Scene
{
public:
	MainMenu();
	~MainMenu();

	void init(const std::string &fileName, Bengine::InputManager* manager, const std::map<std::string, Character *>& characters, const int& screenWidth, const int& screenHeight) override;
	void update() override;
	void doFading() override;
	void enterName(unsigned int keyID);
	void drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight) override;
	void drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight) override;

	void menuInputs();
	void newGameInputs();
	void newGame();

	// Getters
	std::string getPlayerName() { return this->playerName; }
	std::string changeScene() override { return this->nextScene; }
	bool exitGame() override { return this->currentState == MenuState::QUIT; }
private:
	Bengine::InputManager* inputManager; ///< Input manager

	std::string playerName = "Placeholder"; ///< The name of the player
	std::string fileName = "";

	std::string currentBackground = "menu"; ///< The current background of the menu
	std::map<std::string, int> backgrounds; ///< All the backgrounds for the different menu states

	MenuState currentState = MenuState::MENU; ///< The state the menu is currently at
};