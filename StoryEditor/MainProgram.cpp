#include "MainProgram.h"


MainProgram::MainProgram()
{
}


MainProgram::~MainProgram()
{
}


void MainProgram::run()
{
	this->initSystems();

	this->gameLoop();
}


void MainProgram::initSystems()
{
	// Initialize the engine
	Bengine::init();

	// Create the window
	this->window.create("Teemun fantasiat editor", this->screenWidth, this->screenHeight, 0, false);

	// Initialize shaders
	initShaders();

	// Initialize camera
	this->camera.init(this->screenWidth, this->screenHeight);

	this->fontCamera.init(this->screenWidth, this->screenHeight);
	this->fontCamera.setPosition(glm::vec2(this->screenWidth / 2, this->screenHeight / 2));

	// Initialize the spritebatch
	this->spriteBatch.init();
	this->fontBatch.init();

	// Initialize font
	this->spriteFont = new Bengine::SpriteFont("Fonts/Chapaza/Chapaza.ttf", 32);

	// Initialize the scene manager
	this->sceneManager = new SceneManager();

	// Release all keys used in the game because release mode somehow has them already pressed
	this->inputManager.releaseKey(SDL_BUTTON_LEFT);
	this->inputManager.releaseKey(SDL_BUTTON_RIGHT);
}


void MainProgram::initShaders()
{
	this->shaderProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	this->shaderProgram.addAttribute("vertexPosition");
	this->shaderProgram.addAttribute("vertexColor");
	this->shaderProgram.addAttribute("vertexUV");
	this->shaderProgram.linkShaders();
}


void MainProgram::gameLoop()
{
	while (this->currentState != ProgramState::EXIT) {
		while (this->currentState == ProgramState::MAINSCREEN) {
			this->camera.update();
			this->inputManager.update();

			this->processInput();
			this->checkMainScreenInputs();

			this->drawScreen();
		}
		
		while (this->currentState == ProgramState::ADDSCENE) {
			this->camera.update();
			this->fontCamera.update();
			this->inputManager.update();
			this->processInput();
			this->checkSceneCreationScreenInputs();

			this->drawScreen();
		}

		this->currentSceneName = "";
	}
}


void MainProgram::processInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			this->sceneManager->saveScenes("TestDialogs/Monday.yaml");
			this->currentState = ProgramState::EXIT;
			break; 
		case SDL_KEYDOWN:
			this->inputManager.pressKey(event.key.keysym.sym);
			this->onKeyPress(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			this->inputManager.releaseKey(event.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			this->inputManager.setMouseCoords(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
			break;
		case SDL_MOUSEBUTTONDOWN:
			this->inputManager.pressKey(event.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			this->inputManager.releaseKey(event.button.button);
			break;
		}
	}
}


void MainProgram::checkMainScreenInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();

	// Mouse is inside the 'add new' button
	if (mouseCoords.x > 34 && mouseCoords.x < 168) {
		if (mouseCoords.y > this->screenHeight - 68 && mouseCoords.y < this->screenHeight - 30) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);
				this->currentState = ProgramState::ADDSCENE;
			}
		}
	}
}


void MainProgram::checkSceneCreationScreenInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();

	// Get cancel button's location
	static const int cancelButtonX = this->screenWidth / 2 + this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS;
	static const int cancelButtonY = this->screenHeight / 2 + abs(this->BUTTONS_MIDDLE_VERTICAL_RADIUS) - this->BUTTON_HEIGHT;

	// Mouse is inside the 'Cancel' button
	if (mouseCoords.x > cancelButtonX && mouseCoords.x < cancelButtonX + this->BUTTON_WIDTH) {
		if (mouseCoords.y > cancelButtonY && mouseCoords.y < cancelButtonY + this->BUTTON_HEIGHT) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				this->currentState = ProgramState::MAINSCREEN;
			}
		}
	}

	static const int createButtonX = this->screenWidth / 2 - this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS - this->BUTTON_WIDTH;
	static const int createButtonY = this->screenHeight / 2 + abs(this->BUTTONS_MIDDLE_VERTICAL_RADIUS) - this->BUTTON_HEIGHT;

	// Mouse is inside the 'Create' button
	if (mouseCoords.x > createButtonX && mouseCoords.x < createButtonX + this->BUTTON_WIDTH) {
		if (mouseCoords.y > createButtonY && mouseCoords.y < createButtonY + this->BUTTON_HEIGHT) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);
				
				this->sceneManager->addScene(this->currentSceneName);
				this->currentState = ProgramState::MAINSCREEN;
			}
		}
	}
}


void MainProgram::onKeyPress(unsigned int keyID)
{
	// Handle modifying the current scene name being given
	if (this->currentState == ProgramState::ADDSCENE) {
		std::string keyName = SDL_GetKeyName(keyID);
		static std::string allowedKeys[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Space", "Backspace", "Return" };

		bool keyAllowed = false;

		for (std::string name : allowedKeys) {
			if (name == keyName) keyAllowed = true;
		}

		if (!keyAllowed) return;

		// Remove the last character from the player's name
		if (keyName == "Backspace") {
			this->currentSceneName = this->currentSceneName.substr(0, this->currentSceneName.size() - 1);
		}
		// Add a space to the player's name
		else if (keyName == "Space" && this->currentSceneName.length() < 16) {
			if (this->currentSceneName.length() > 1) this->currentSceneName += " ";
		}
		// Confirm the player's name
		else if (keyName == "Return") {
			if (this->currentSceneName.length() >= 3 && this->currentSceneName.length() <= 16) {
				std::cout << "Name was valid.\n";
			}
		}
		// Add the character to the player's name
		else {
			if (this->currentSceneName.length() < 16) {
				this->currentSceneName += static_cast<char>(keyID);
			}
		}

		// Capitalize first letters of the name
		this->currentSceneName[0] = toupper(this->currentSceneName[0]);

		for (unsigned i = 0; i < this->currentSceneName.length(); i++) {
			if (this->currentSceneName[i] == ' ') {
				if (i != this->currentSceneName.length() - 1) {
					this->currentSceneName[i + 1] = toupper(this->currentSceneName[i + 1]);
				}
			}
		}
	}
}


void MainProgram::drawScreen()
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
	glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

	this->spriteBatch.begin();

	if (this->currentState == ProgramState::MAINSCREEN) this->drawMainScreen();
	else if (this->currentState == ProgramState::ADDSCENE) this->drawSceneCreationScreen();

	this->spriteBatch.end();
	this->spriteBatch.renderBatch();

	if (this->currentState == ProgramState::ADDSCENE) this->drawSceneCreationScreenTexts();
	else if (this->currentState == ProgramState::MAINSCREEN) this->drawMainScreenTexts();

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the shader program
	this->shaderProgram.unuse();

	// Swap the buffer
	this->window.swapBuffer();
}


void MainProgram::drawMainScreen()
{
	static const unsigned int main_background = Bengine::ResourceManager::getTexture("Textures/main-bg.png").id;
	static const unsigned int add_new_button = Bengine::ResourceManager::getTexture("Textures/addnew.png").id;
	static const unsigned int sceneBlock = Bengine::ResourceManager::getTexture("Textures/scene-block.png").id;

	static const glm::vec4 bgDestRect(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);
	static const glm::vec4 addNewDestRect(-this->screenWidth / 2 + 34, -this->screenHeight / 2 + 30, 134, 34);
	static glm::vec4 sceneBlockDestRect(-this->screenWidth / 2 + 10, this->screenHeight / 2 - 195, 179, 77);
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	this->spriteBatch.draw(
		bgDestRect,
		uvRect,
		main_background,
		0.0f,
		color
	);
	this->spriteBatch.draw(
		addNewDestRect,
		uvRect,
		add_new_button,
		0.0f,
		color
	);

	std::map<int, std::string> scenes = this->sceneManager->getScenes();
	glm::vec4 originalDestRect = sceneBlockDestRect;
	for (unsigned i = 0; i < scenes.size(); i++) {
		this->spriteBatch.draw(
			sceneBlockDestRect,
			uvRect,
			sceneBlock,
			0.0f,
			color
		);
		sceneBlockDestRect.y -= 100;
	}
	sceneBlockDestRect = originalDestRect;
}


void MainProgram::drawSceneCreationScreen()
{
	static const unsigned int background = Bengine::ResourceManager::getTexture("Textures/newscene.png").id;
	static const unsigned int cancelButton = Bengine::ResourceManager::getTexture("Textures/cancel-button.png").id;
	static const unsigned int createButton = Bengine::ResourceManager::getTexture("Textures/create-button.png").id;
	
	static const glm::vec4 bgDestRect(-this->screenWidth / 2, -this->screenHeight / 2, this->screenWidth, this->screenHeight);
	static const glm::vec4 cancelBtnDestRect(this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS, this->BUTTONS_MIDDLE_VERTICAL_RADIUS, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);
	static const glm::vec4 createBtnDestRect(-this->BUTTONS_MIDDLE_HORIZONTAL_RADIUS - 149, this->BUTTONS_MIDDLE_VERTICAL_RADIUS, this->BUTTON_WIDTH, this->BUTTON_HEIGHT);

	static const glm::vec4 addNewDestRect(-this->screenWidth / 2 + 50, -this->screenHeight / 2 + 30, 149, 38);
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	// Background (banner, bg color)
	this->spriteBatch.draw(
		bgDestRect,
		uvRect,
		background,
		0.0f,
		color
	);

	// Cancel button
	this->spriteBatch.draw(
		cancelBtnDestRect,
		uvRect,
		cancelButton,
		0.0f,
		color
	);

	// Create button
	this->spriteBatch.draw(
		createBtnDestRect,
		uvRect,
		createButton,
		0.0f,
		color
	);
}


void MainProgram::drawMainScreenTexts()
{
	// Make the buffer that will hold the text
	char buffer[256];

	// Set the camera matrix
	GLint pLocation = this->shaderProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = this->fontCamera.getCameraMatrix();

	// Send the camera matrix
	glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

	this->fontBatch.begin();

	std::map<int, std::string> scenes = this->sceneManager->getScenes();
	int currentY = 152;
	for (unsigned i = 0; i < scenes.size(); i++) {
		// Fill the buffer with the text
		sprintf_s(buffer, "%s", scenes[i].c_str());

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(20, this->screenHeight - currentY),
			glm::vec2(0.7f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		currentY += 100;
	}

	this->fontBatch.end();
	this->fontBatch.renderBatch();
}


void MainProgram::drawSceneCreationScreenTexts()
{
	// Make the buffer that will hold the text
	char buffer[256];

	// Set the camera matrix
	GLint pLocation = this->shaderProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = this->fontCamera.getCameraMatrix();

	// Send the camera matrix
	glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

	this->fontBatch.begin();

	// Fill the buffer with the text
	sprintf_s(buffer, "%s", this->currentSceneName.c_str());

	this->spriteFont->draw(
		this->fontBatch,
		buffer,
		glm::vec2(this->screenWidth / 2, this->screenHeight / 2 + 50),
		glm::vec2(1.0f),
		0.0f,
		Bengine::ColorRGBA8(0, 0, 0, 255),
		Bengine::Justification::MIDDLE
	);

	this->fontBatch.end();
	this->fontBatch.renderBatch();
}
