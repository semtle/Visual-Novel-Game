#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <vector>
#include <map>

// For checking if character is alphabetic
#include <stdio.h>
#include <ctype.h>

#include <Bengine/Bengine.h>
#include <Bengine/Window.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteFont.h>

#include "Scene.h"
#include "MainMenu.h"
#include "Day.h"
#include "Character.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	// Starts up the game
	void run();

	// Initializes all core systems
	void initSystems();

	// Initializes shaders
	void initShaders();

	// Initialize all scenes
	void initScenes();

	void initDays();

	// The main game loop
	void gameLoop();

	// While in the play state
	void play();

	// Handles all user input
	void processInput();

	// Handle key down events for name entering
	void handleKeydown(SDL_Event event);

	// Draws the game to the screen
	void drawGame();
private:
	std::string playerName = "";

	Bengine::Window window; ///< Main window

	Bengine::SpriteBatch spriteBatch;

	Bengine::SpriteFont* spriteFont; ///< The font of the text

	Bengine::Camera2D camera; ///< Main camera
	Bengine::Camera2D hudCamera; ///< HUD camera

	Bengine::GLSLProgram shaderProgram; ///< Shaders

	Bengine::InputManager inputManager; ///< Main input manager

	GameState gameState = GameState::PLAY; ///< The current state of the game

	std::map<std::string, Character*> characters;
	std::string currentScene;
	MainMenu menuScene; ///< The main menu's scene
	Day monday;
	std::map<std::string, Scene*> scenes; ///< All the scenes in the game
	std::map<std::string, Day*> days;

	int screenWidth = 800;
	int screenHeight = 600;
};