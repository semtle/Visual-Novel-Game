#include "MainGame.h"


MainGame::MainGame()
{
}


MainGame::~MainGame()
{
}


void MainGame::run()
{
	initSystems();

	gameLoop();
}


void MainGame::initSystems()
{
	// Initialize the engine
	Bengine::init();

	// Create the window
	this->window.create("Teemun fantasiat", this->screenWidth, this->screenHeight, 0, false);
	
	// Initialize shaders
	initShaders();

	// Initialize camera
	this->camera.init(this->screenWidth, this->screenHeight);

	// Initialize HUD camera
	this->hudCamera.init(this->screenWidth, this->screenHeight);
	this->hudCamera.setPosition(glm::vec2(this->screenWidth / 2, this->screenHeight / 2));

	// Initialize the spritebatch
	this->spriteBatch.init();

	// Initialize font
	this->spriteFont = new Bengine::SpriteFont("Fonts/Chapaza/Chapaza.ttf", 32);

	// Initialize all scenes
	initScenes();

	// Release all keys used in the game because release mode somehow has them already pressed
	this->inputManager.releaseKey(SDL_BUTTON_LEFT);
	this->inputManager.releaseKey(SDL_BUTTON_RIGHT);
}


void MainGame::initShaders()
{
	this->shaderProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	this->shaderProgram.addAttribute("vertexPosition");
	this->shaderProgram.addAttribute("vertexColor");
	this->shaderProgram.addAttribute("vertexUV");
	this->shaderProgram.linkShaders();
}


void MainGame::initScenes()
{
	// Start from menu when the game runs
	this->currentScene = "menu";

	// Create the menu
	this->menuScene.init(this->fileName, &this->inputManager, this->characters, this->screenWidth, this->screenHeight);
	this->scenes.emplace("menu", &this->menuScene);

	// Boys
	this->characters.emplace("Makoto-kun", new Character("Makoto-kun")); ///< Normal boy
	this->characters.emplace("Teemu-kun", new Character("Teemu-kun"));   ///< Hidden nerd boy
	this->characters.emplace("Iwao-kun", new Character("Iwao-kun"));     ///< Badass boy
	this->characters.emplace("Hideo-kun", new Character("Hideo-kun"));   ///< Model boy

	// Girls
	this->characters.emplace("Fumiko-san", new Character("Fumiko-san"));   ///< Cute girl
	this->characters.emplace("Maiko-san", new Character("Maiko-san"));     ///< Energic girl
	this->characters.emplace("Michiko-san", new Character("Michiko-san")); ///< Serious girl
}


void MainGame::initDays()
{
	this->days.emplace(this->fileName, new Day());
	this->days[fileName]->setDay("Monday");
	this->days[fileName]->setPlayerName(this->playerName);

	// Initialize all the scenes
	for (std::pair<std::string, Day*> day : this->days) {
		day.second->setPlayerName(this->menuScene.getPlayerName());
		day.second->init(day.first, &this->inputManager, this->characters, this->screenWidth, this->screenHeight);

		this->scenes.emplace(day.first, day.second);
	}
}


void MainGame::gameLoop()
{
	while (this->gameState != GameState::EXIT) {
		while (this->gameState == GameState::PLAY) play();
	}
}


void MainGame::play()
{
	this->camera.update();
	this->hudCamera.update();
	processInput();
	this->inputManager.update();

	drawGame();

	Scene* currScene = this->scenes.find(this->currentScene)->second;

	currScene->update();

	if (currScene->exitGame()) this->gameState = GameState::EXIT;

	// Change to the next scene if the current scene is over
	std::string nextScene = currScene->changeScene();
	if (nextScene != "") {
		if (this->currentScene == "menu") {
			initDays();
		}

		this->currentScene = nextScene;
	}
}


void MainGame::processInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			this->gameState = GameState::EXIT;
			break;
		case SDL_KEYDOWN:
			this->inputManager.pressKey(event.key.keysym.sym);
			this->menuScene.enterName(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			this->inputManager.releaseKey(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			this->inputManager.pressKey(event.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			this->inputManager.releaseKey(event.button.button);
			break;
		case SDL_MOUSEMOTION:
			this->inputManager.setMouseCoords(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
			break;
		}
	}
}


void MainGame::handleKeydown(SDL_Event event)
{
}


void MainGame::drawGame()
{
	// Set the base depth to 1.0
	glClearDepth(1.0f);

	// Clear the depth and color buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader program
	this->shaderProgram.use();

	// Set the first texture to be active
	glActiveTexture(GL_TEXTURE0);

	GLint textureLocation = this->shaderProgram.getUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	// Set the camera matrix
	GLint pLocation = this->shaderProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = this->camera.getCameraMatrix();

	// Send the camera matrix
	// AHHHHHHHHH REMEMBER TO SEND BEFORE DRAWING THE FUCKING LEVEL JESUS FUCKING CHRIST GONNA KIKLL MY SELF
	glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);
	
	this->spriteBatch.begin(Bengine::GlyphSortType::NONE);
	
	if (!this->loading) {
		this->scenes.find(this->currentScene)->second->drawImages(this->spriteBatch, &this->hudCamera, &this->shaderProgram, this->screenWidth, this->screenHeight);

		this->spriteBatch.end();
		this->spriteBatch.renderBatch();

		// Draw all texts in the current scene
		this->scenes.find(this->currentScene)->second->drawTexts(this->spriteFont, &this->shaderProgram, this->screenWidth, this->screenHeight);
	}
	else {
		this->spriteBatch.draw(
			glm::vec4(0, 0, 150, 150),
			glm::vec4(0.0f, 0.0f, 1.0f, 2.0f),
			Bengine::ResourceManager::getTexture("Textures/Visuals/LoadingSpinInner.png").id,
			0.0f,
			Bengine::ColorRGBA8(255, 255, 255, 255)
		);

		this->spriteBatch.end();
		this->spriteBatch.renderBatch();
	}

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the shader program
	this->shaderProgram.unuse();

	// Swap the buffer
	this->window.swapBuffer();
}