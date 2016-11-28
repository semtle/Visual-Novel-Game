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
	this->spriteFont = new Bengine::SpriteFont("Fonts/timeburnernormal.ttf", 32);

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
		while (this->currentState == ProgramState::FILESELECT) {
			this->camera.update();
			this->fontCamera.update();
			this->inputManager.update();

			this->processInput();
			this->checkFileSelectInputs();

			this->drawScreen();
		}

		while (this->currentState == ProgramState::MAINSCREEN) {
			this->camera.update();
			this->inputManager.update();

			this->processInput();
			this->checkMainScreenInputs();
			this->update();

			this->drawScreen();
		}
		
		while (this->currentState == ProgramState::ADDSCENE || this->currentState == ProgramState::ADD_DIALOGUE) {
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


void MainProgram::update()
{
	this->shownSceneBlockPositions.clear();

	int counter = 0;

	// Store the positions of the scene boxes to check which box was clicked on
	// Doing this every frame so that the positions are always updated.
	for (int idx : this->shownSceneIndexes) {
		this->shownSceneBlockPositions.push_back(
			std::pair<int, glm::vec2>
			(
				idx,
				glm::vec2(this->sceneBlockDrawStartPos.x, this->sceneBlockDrawStartPos.y - (this->sceneBlockPositionDiff * counter))
			)
		);

		counter++;
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

			std::cout << "Current scene name: " << this->currentSceneName << "\n";
			std::cout << "Current dialogue name: " << this->currentDialogueName << "\n";
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


void MainProgram::checkFileSelectInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();
}


void MainProgram::checkMainScreenInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();

	glm::vec4 dim = this->getInputDimensions(this->addNewDestRect);
	
	// Mouse is inside the 'add new' button
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);
				this->currentState = ProgramState::ADDSCENE;
			}
		}
	}

	dim = this->getInputDimensions(this->upArrowDestDect);

	// Mouse is inside the up arrow
	if (this->currentSceneListIdx > 0) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->currentSceneListIdx--;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->downArrowDestDect);

	// Mouse is inside the down arrow
	if (this->sceneManager->getScenes().size() > 5 && this->currentSceneListIdx < this->sceneManager->getScenes().size() - 5) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->currentSceneListIdx++;
				}
			}
		}
	}

	// Check if any of the scene boxes were clicked on
	for (unsigned i = 0; i < this->shownSceneBlockPositions.size(); i++) {
		dim = this->getInputDimensions(glm::vec4(this->shownSceneBlockPositions[i].second, this->SCENE_BOX_WIDTH, this->SCENE_BOX_HEIGHT));

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					std::cout << "Clicked on scene box\n";
				}
			}
		}
	}
}


void MainProgram::checkSceneCreationScreenInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();

	glm::vec4 dim = this->getInputDimensions(this->cancelBtnDestRect);

	// Mouse is inside the 'Cancel' button
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				this->currentState = ProgramState::MAINSCREEN;
			}
		}
	}

	dim = this->getInputDimensions(this->createBtnDestRect);

	// Mouse is inside the 'Create' button
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				if (this->currentState == ProgramState::ADDSCENE) {
					this->sceneManager->addScene(this->currentSceneName);
				}
				else if (this->currentState == ProgramState::ADD_DIALOGUE) {
					this->sceneManager->addDialogue(this->selectedSceneIdx, &this->currentDialogue);
				}

				this->currentState = ProgramState::MAINSCREEN;
			}
		}
	}

	// When pressing enter, submit the new scene
	if (this->inputManager.isKeyDown(SDLK_RETURN)) {
		this->inputManager.releaseKey(SDL_BUTTON_LEFT);

		if (this->currentState == ProgramState::ADDSCENE) {
			this->sceneManager->addScene(this->currentSceneName);
		}
		else if (this->currentState == ProgramState::ADD_DIALOGUE) {
			this->sceneManager->addDialogue(this->selectedSceneIdx, &this->currentDialogue);
		}

		this->currentState = ProgramState::MAINSCREEN;
	}
}


void MainProgram::onKeyPress(unsigned int keyID)
{
	if (this->currentState == ProgramState::ADD_DIALOGUE || this->currentState == ProgramState::ADDSCENE) {
		// Handle modifying the current scene name being given
		std::string keyName = SDL_GetKeyName(keyID);
		static std::string allowedKeys[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Space", "Backspace", "Return" };

		bool keyAllowed = false;

		for (std::string name : allowedKeys) {
			if (name == keyName) keyAllowed = true;
		}

		if (!keyAllowed) return;

		// Remove the last character from the player's name
		if (keyName == "Backspace") {
			if (this->currentState == ProgramState::ADDSCENE) {
				this->currentSceneName = this->currentSceneName.substr(0, this->currentSceneName.size() - 1);
			}
			else {
				this->currentDialogueName = this->currentDialogueName.substr(0, this->currentDialogueName.size() - 1);
			}
		}

		// Add a space to the name
		if (this->currentState == ProgramState::ADDSCENE) {
			if (keyName == "Space" && this->currentSceneName.length() < 16) {
				if (this->currentSceneName.length() > 1) this->currentSceneName += " ";
			}
		}

		else if (this->currentState == ProgramState::ADD_DIALOGUE) {
			if (keyName == "Space" && this->currentDialogueName.length() < 16) {
				if (this->currentDialogueName.length() > 1) this->currentDialogueName += " ";
			}
		}

		// Confirm the player's name
		if (keyName == "Return") {
			if (this->currentState == ProgramState::ADDSCENE) {
				if (this->currentSceneName.length() >= 3 && this->currentSceneName.length() <= 16) {
					std::cout << "Name was valid.\n";
				}
			}
			else {
				if (this->currentDialogueName.length() >= 3 && this->currentDialogueName.length() <= 16) {
					std::cout << "Name was valid.\n";
				}
			}
		}

		// Add the character to the player's name
		if (keyName != "Return" && keyName != "Space" && keyName != "Backspace") {
			if (this->currentState == ProgramState::ADDSCENE) {
				if (this->currentSceneName.length() < 16) {
					this->currentSceneName += static_cast<char>(keyID);
				}
			}
			else {
				if (this->currentDialogueName.length() < 16) {
					this->currentDialogueName += static_cast<char>(keyID);
				}
			}
		}

		// Capitalize first letters of the name
		if (this->currentSceneName.length() > 0) {
			this->currentSceneName[0] = toupper(this->currentSceneName[0]);
		}
		if (this->currentDialogueName.length() > 0) {
			this->currentDialogueName[0] = toupper(this->currentDialogueName[0]);
		}

		// Scene name
		for (unsigned i = 0; i < this->currentSceneName.length(); i++) {
			if (this->currentSceneName[i] == ' ') {
				if (i != this->currentSceneName.length() - 1) {
					this->currentSceneName[i + 1] = toupper(this->currentSceneName[i + 1]);
				}
			}
		}

		// Dialogue name
		for (unsigned i = 0; i < this->currentDialogueName.length(); i++) {
			if (this->currentDialogueName[i] == ' ') {
				if (i != this->currentDialogueName.length() - 1) {
					this->currentDialogueName[i + 1] = toupper(this->currentDialogueName[i + 1]);
				}
			}
		}
	}
}

void MainProgram::drawScreen()
{
	// Set the base depth to 1.0
	glClearDepth(1.0f);

	// Clear the depth and this->color buffers
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


void MainProgram::drawFileSelectScreen()
{
	// Empty
}


void MainProgram::drawMainScreen()
{
	static const unsigned int main_background = Bengine::ResourceManager::getTexture("Textures/main-bg.png").id;
	static const unsigned int arrow = Bengine::ResourceManager::getTexture("Textures/arrow.png").id;
	static const unsigned int add_new_button = Bengine::ResourceManager::getTexture("Textures/addnew.png").id;
	static const unsigned int sceneBox = Bengine::ResourceManager::getTexture("Textures/scene-box.png").id;

	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> allScenes = this->sceneManager->getScenes();

	// BG
	this->spriteBatch.draw(
		this->mainBgDestRect,
		this->mainUvRect,
		main_background,
		0.0f,
		this->color
	);

	// Add new -button
	this->spriteBatch.draw(
		this->addNewDestRect,
		this->mainUvRect,
		add_new_button,
		0.0f,
		this->color
	);

	// Up arrow
	if (this->currentSceneListIdx > 0) {
		this->spriteBatch.draw(
			this->upArrowDestDect,
			this->upArrowUvRect,
			arrow,
			0.0f,
			this->color
		);
	}

	// Down arrow
	if (allScenes.size() > 5 && this->currentSceneListIdx < allScenes.size() - 5) {
		this->spriteBatch.draw(
			this->downArrowDestDect,
			this->mainUvRect,
			arrow,
			0.0f,
			this->color
		);
	}

	// Draw dialogue editor
	if (this->selectedSceneIdx != -1) this->drawCurrentDialogue();

	// If no scene is selected, show list of scenes
	if (this->selectedSceneIdx == -1) {
		std::map<int, std::pair<std::string, std::vector<Dialogue *>>> shownScenes = this->getShownScenes(allScenes);

		glm::vec4 originalDestRect = sceneBoxDestRect;
		for (unsigned i = 0; i < shownScenes.size(); i++) {
			this->spriteBatch.draw(
				this->sceneBoxDestRect,
				this->mainUvRect,
				sceneBox,
				0.0f,
				this->color
			);
			sceneBoxDestRect.y -= this->sceneBlockPositionDiff;
		}
		sceneBoxDestRect = originalDestRect;
	}
	// If a scene is selected, show list of the dialogues
	else {

	}
}


void MainProgram::drawCurrentDialogue()
{
	static const unsigned int blackBox = Bengine::ResourceManager::getTexture("Textures/schoolyard.png").id;
	static const unsigned int textBox = Bengine::ResourceManager::getTexture("Textures/TextBoxBlue.png").id;

	/*
	this->spriteBatch.draw(
		this->blackBoxDestRect,
		this->mainUvRect,
		blackBox,
		0.0f,
		this->color
	);

	this->spriteBatch.draw(
		glm::vec4(185, -290, 165, 440),
		this->mainUvRect,
		laugh,
		0.0f,
		this->color
	);

	this->spriteBatch.draw(
		glm::vec4(-135, -290, 165, 440),
		glm::vec4(0.0f, 0.0f, -1.0f, 1.0f),
		laugh,
		0.0f,
		this->color
	);

	this->spriteBatch.draw(
		glm::vec4(-this->screenWidth / 2 + 200, -270, 600, 500),
		this->mainUvRect,
		box,
		0.0f,
		this->color
	);
	*/
}


void MainProgram::drawSceneCreationScreen()
{
	static const unsigned int addSceneBackground = Bengine::ResourceManager::getTexture("Textures/newscene.png").id;
	static const unsigned int addDialogueBackground = Bengine::ResourceManager::getTexture("Textures/newdialogue.png").id;
	static const unsigned int cancelButton = Bengine::ResourceManager::getTexture("Textures/cancel-button.png").id;
	static const unsigned int createButton = Bengine::ResourceManager::getTexture("Textures/create-button.png").id;

	// Background (banner, bg this->color)
	if (this->currentState == ProgramState::ADDSCENE) {
		this->spriteBatch.draw(
			this->mainBgDestRect,
			this->mainUvRect,
			addSceneBackground,
			0.0f,
			this->color
		);
	}
	else if (this->currentState == ProgramState::ADD_DIALOGUE) {
		this->spriteBatch.draw(
			this->mainBgDestRect,
			this->mainUvRect,
			addDialogueBackground,
			0.0f,
			this->color
		);
	}

	// Cancel button
	this->spriteBatch.draw(
		this->cancelBtnDestRect,
		this->mainUvRect,
		cancelButton,
		0.0f,
		this->color
	);

	// Create button
	this->spriteBatch.draw(
		this->createBtnDestRect,
		this->mainUvRect,
		createButton,
		0.0f,
		this->color
	);
}


void MainProgram::drawFileSelectTexts()
{
	// Empty
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

	// Get scenes to be shown
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> scenes = this->getShownScenes(this->sceneManager->getScenes());

	int currentY = 196;
	for (unsigned i = 0; i < scenes.size(); i++) {
		// Fill the buffer with the text
		sprintf_s(buffer, "%s", scenes[i].first.c_str());

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(25, this->screenHeight - currentY),
			glm::vec2(0.7f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		currentY += 90;
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


std::map<int, std::pair<std::string, std::vector<Dialogue *>>> MainProgram::getShownScenes(std::map<int, std::pair<std::string, std::vector<Dialogue *>>> allScenes)
{
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> shownScenes;
	std::vector<int> indexes;

	// If there's more scenes that can be shown, only pick 5
	if (allScenes.size() > 5) {
		// Get 4 items from the map
		int counter = 0;

		for (auto it = allScenes.begin(); it != allScenes.end(); it++) {
			if (counter == this->currentSceneListIdx) {
				// Put the 4 scenes that will be shown into the 'shownScenes' vector
				for (unsigned i = 0; i < 5; i++) {
					shownScenes.emplace(i, it->second);
					indexes.push_back(it->first);

					it++;
				}

				break;
			}

			counter++;
		}
	}

	this->shownSceneIndexes.clear();

	if (shownScenes.size() == 0) {
		// Store shown scene indexes
		for (auto it = allScenes.begin(); it != allScenes.end(); it++) {
			this->shownSceneIndexes.push_back(it->first);
		}

		return allScenes;
	}
	else {
		// Store shown scene indexes
		this->shownSceneIndexes = indexes;

		return shownScenes;
	}
}


glm::vec4 MainProgram::getInputDimensions(glm::vec4 texture, bool swapy)
{
	texture.x = texture.x + this->screenWidth / 2;

	if (swapy) {
		texture.y = abs((texture.y + this->screenHeight / 2) - this->screenHeight) - texture.a; // Flips the y value
	}

	return texture;
}
