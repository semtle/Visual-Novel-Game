#include "MainProgram.h"

#define ARRSIZE(arr) (sizeof(arr)/sizeof(*(arr)))


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

	std::cout << "Creating window...\n";

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
			this->fontCamera.update();
			this->inputManager.update();

			this->processInput();
			this->checkMainScreenInputs();
			this->updateBoxes();

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
		this->currentDialogueName = "";
	}
}


void MainProgram::updateBoxes()
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

	this->showDialogueBoxPositions.clear();

	counter = 0;

	// Store the positions of the scene boxes to check which box was clicked on
	// Doing this every frame so that the positions are always updated.
	for (int idx : this->shownDialogueIndexes) {
		this->showDialogueBoxPositions.push_back(
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

	glm::vec4 dim = this->getInputDimensions(this->exitBtnDestRect);

	// Exit button
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->currentState = ProgramState::EXIT;
			}
		}
	}

	dim = this->getInputDimensions(this->openFileBtnDestRect);

	// Open File button
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				// Open up the file explorer to pick a yaml file
				std::wstring wFilePath = this->getOpenFileName(NULL, false);

				// Convert the filepath from a wide string to a regural string
				std::string filePath = "";
				for (char c : wFilePath) {
					filePath += c;
				}

				std::cout << "File path: " << filePath << "\n";

				size_t pos = filePath.find("Dialogues");

				if (filePath.length() > 0)
					this->currentFileName = filePath.substr(pos + 10);
			}
		}
	}

	dim = this->getInputDimensions(this->editFileBtnDestRect);

	// Edit File button
	if (this->currentFileName.length() > 0) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->currentState = ProgramState::MAINSCREEN;
				}
			}
		}
	}
}


void MainProgram::checkMainScreenInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();

	glm::vec4 dim = this->getInputDimensions(this->talkerBoxDestRect);

	// If clicked outside of talker box, don't allow setting the name
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->clickedOnTalkerBox = false;
		}
	}

	dim = this->getInputDimensions(this->textBoxDestRect);

	// Check if mouse clicked outside of dialogue box
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->clickedOnDialogueBox = false;
		}
	}

	dim = this->getInputDimensions(this->addNewDestRect);
	
	// Mouse is inside the 'add new' button
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				if (this->selectedSceneIdx == -1) {
					this->currentState = ProgramState::ADDSCENE;
				}
				else {
					this->currentDialogue = nullptr;
					this->currentState = ProgramState::ADD_DIALOGUE;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->upArrowDestDect);

	// Mouse is inside the up arrow
	if ((this->currentSceneListIdx > 0 && this->selectedSceneIdx == -1) || (this->currentDialogueListIdx > 0 && this->selectedSceneIdx != -1)) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					if (this->selectedSceneIdx == -1)
						this->currentSceneListIdx--;
					else
						this->currentDialogueListIdx--;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->downArrowDestDect);

	int dialoguesSize = 0;
	if (this->selectedSceneIdx != -1)
		dialoguesSize = this->sceneManager->getDialogues(this->selectedSceneIdx).size();

	// Mouse is inside the down arrow
	if ((this->sceneManager->getScenes().size() > 5 && this->currentSceneListIdx < this->sceneManager->getScenes().size() - 5 && this->selectedSceneIdx == -1)
		|| (dialoguesSize > 5 && this->currentDialogueListIdx < dialoguesSize - 5 && this->selectedSceneIdx != -1)) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					if (this->selectedSceneIdx == -1)
						this->currentSceneListIdx++;
					else {
						this->currentDialogueListIdx++;
						std::cout << "Added index\n";
					}
				}
			}
		}
	}

	// Check if any of the scene boxes were clicked on
	if (this->selectedSceneIdx == -1) {
		for (unsigned i = 0; i < this->shownSceneBlockPositions.size(); i++) {
			dim = this->getInputDimensions(glm::vec4(this->shownSceneBlockPositions[i].second, this->SCENE_BOX_WIDTH, this->SCENE_BOX_HEIGHT));

			if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
				if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
					if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
						this->inputManager.releaseKey(SDL_BUTTON_LEFT);

						// Open the clicked scene
						this->selectedSceneIdx = this->shownSceneIndexes[i];
					}
				}
			}
		}
	}
	else {
		// Check if any of the dialogue boxes were clicked on
		for (unsigned i = 0; i < this->showDialogueBoxPositions.size(); i++) {
			dim = this->getInputDimensions(glm::vec4(this->showDialogueBoxPositions[i].second, this->SCENE_BOX_WIDTH, this->SCENE_BOX_HEIGHT));

			if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
				if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
					if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
						this->inputManager.releaseKey(SDL_BUTTON_LEFT);
						std::cout << "Was clicked.\n";

						// Open the clicked scene
						this->selectedDialogueIdx = this->shownDialogueIndexes[i];

						// Set the current dialogue
						this->currentDialogue = this->sceneManager->getDialogues(this->selectedSceneIdx)[this->selectedDialogueIdx];
					}
				}
			}
		}
	}

	dim = getInputDimensions(this->arrowLeftDestRect);

	// Check if the back arrow was clicked on
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				this->selectedSceneIdx = -1;
				this->selectedDialogueIdx = -1;
				this->currentDialogueListIdx = 0;

				this->currentDialogue = nullptr;
			}
		}
	}

	// Background button
	if (this->currentDialogue != nullptr) {
		dim = this->getInputDimensions(this->bgBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					// Open up the file explorer to pick a background image
					std::wstring wFilePath = this->getOpenFileName(NULL);

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (char c : wFilePath) {
						filePath += c;
					}

					// Set the background
					if (filePath.length() > 0) {
						this->currentDialogue->background = filePath;
					}
				}
			}
		}
	}

	// Char 1 button
	if (this->currentDialogue != nullptr && this->currentDialogue->background != "") {
		dim = this->getInputDimensions(this->char1BtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					// Open up the file explorer to pick a background image
					std::wstring wFilePath = this->getOpenFileName(NULL);

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (char c : wFilePath) {
						filePath += c;
					}

					// Set the background
					if (filePath.length() > 0) {
						this->currentDialogue->left = filePath;
					}
				}
			}
		}
	}

	// Char 2 button
	if (this->currentDialogue != nullptr && this->currentDialogue->background != "") {
		dim = this->getInputDimensions(this->char2BtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					// Open up the file explorer to pick a background image
					std::wstring wFilePath = this->getOpenFileName(NULL);

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (char c : wFilePath) {
						filePath += c;
					}

					// Set the background
					if (filePath.length() > 0) {
						this->currentDialogue->right = filePath;
					}
				}
			}
		}
	}

	dim = this->getInputDimensions(this->textBoxDestRect);

	// Dialogue box
	if (this->currentDialogue != nullptr && this->currentDialogue->background != "") {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnDialogueBox = true;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->talkerBoxDestRect);

	// Talker box
	if (this->currentDialogue != nullptr && this->currentDialogue->background != "") {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnTalkerBox = true;
				}
			}
		}
	}

	if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) this->inputManager.releaseKey(SDL_BUTTON_LEFT);
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
				
				// Set the current dialogue to be the last one when cancelling
				if (this->selectedSceneIdx != -1) {
					std::vector<Dialogue *> dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);
					this->currentDialogue = dialogues[dialogues.size() - 1];
				}
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
					this->submitDialogue();
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
			this->submitDialogue();
		}

		this->currentState = ProgramState::MAINSCREEN;
	}
}


void MainProgram::onKeyPress(unsigned int keyID)
{
	// This function handles writing text in many different places in the editor
	if (this->currentState == ProgramState::ADD_DIALOGUE || this->currentState == ProgramState::ADDSCENE || this->clickedOnTalkerBox || this->clickedOnDialogueBox) {
		// Handle modifying the current scene name being given
		std::string keyName = SDL_GetKeyName(keyID);
		static std::string allowedKeys[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "-", "Space", ".", ",", "+", "1", "Backspace", "Return" };

		bool keyAllowed = false;

		for (std::string name : allowedKeys) {
			if (name == keyName) keyAllowed = true;
		}

		if (!keyAllowed) return;

		// Remove the last character from the player's name
		if (keyName == "Backspace") {
			// Scene name
			if (this->currentState == ProgramState::ADDSCENE) {
				this->currentSceneName = this->currentSceneName.substr(0, this->currentSceneName.size() - 1);
			}
			// Talker
			else if (this->currentDialogue != nullptr && this->clickedOnTalkerBox) {
				this->currentDialogue->talking = this->currentDialogue->talking.substr(0, this->currentDialogue->talking.size() - 1);
			}
			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
				this->currentDialogue->message = this->currentDialogue->message.substr(0, this->currentDialogue->message.size() - 1);
			}
			// Dialogue name
			else {
				this->currentDialogueName = this->currentDialogueName.substr(0, this->currentDialogueName.size() - 1);
			}
		}

		// Add a space to the text
		// Scene name
		if (this->currentState == ProgramState::ADDSCENE) {
			if (keyName == "Space" && this->currentSceneName.length() < 16) {
				if (this->currentSceneName.length() > 0) this->currentSceneName += " ";
			}
		}

		// Dialogue name
		else if (this->currentState == ProgramState::ADD_DIALOGUE) {
			if (keyName == "Space" && this->currentDialogueName.length() < 16) {
				if (this->currentDialogueName.length() > 0) this->currentDialogueName += " ";
			}
		}

		// Message
		else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
			if (keyName == "Space") {
				this->currentDialogue->message += " ";
			}
		}

		// Validate the text
		if (keyName == "Return") {
			// Scene name
			if (this->currentState == ProgramState::ADDSCENE) {
				if (this->currentSceneName.length() >= 3 && this->currentSceneName.length() <= 16) {
					std::cout << "Name was valid.\n";
				}
			}
			// Talker
			else if (this->currentDialogue != nullptr && this->clickedOnTalkerBox) {
				this->clickedOnTalkerBox = false;
			}
			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
				this->clickedOnDialogueBox = false;
			}
			// Dialogue name
			else {
				if (this->currentDialogueName.length() >= 3 && this->currentDialogueName.length() <= 16) {
					std::cout << "Name was valid.\n";
				}
			}
		}

		// Add the character to the player's name
		if (keyName != "Return" && keyName != "Space" && keyName != "Backspace") {
			// Scene name
			if (this->currentState == ProgramState::ADDSCENE && keyName != "+" && keyName != "1" && keyName != ",") {
				if (this->currentSceneName.length() < 16) {
					this->currentSceneName += static_cast<char>(keyID);
				}
			}
			// Talker
			else if (this->clickedOnTalkerBox && keyName != "+" && keyName != "1" && keyName != ",") {
				if (this->currentDialogue != nullptr && this->currentDialogue->talking.length() < 11) {
					this->currentDialogue->talking += static_cast<char>(keyID);
				}
			}
			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
				// Question mark
				if (keyName == "+" && (this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) {
					this->currentDialogue->message += "?";
				}
				// Exclamation mark
				else if (keyName == "1" && (this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) {
					this->currentDialogue->message += "!";
				}
				else {
					this->currentDialogue->message += ((this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) ? toupper(static_cast<char>(keyID)) : static_cast<char>(keyID);
				}
			}
			// Dialogue name
			else {
				if (this->currentDialogueName.length() < 16 && keyName != "+" && keyName != "1" && keyName != ",") {
					this->currentDialogueName += static_cast<char>(keyID);
				}
			}
		}

		// Capitalize first letter of the name
		if (this->currentDialogue != nullptr && this->clickedOnTalkerBox && this->currentDialogue->talking.length() > 0) {
			this->currentDialogue->talking[0] = toupper(this->currentDialogue->talking[0]);
		}

		if (!this->clickedOnTalkerBox && !this->clickedOnDialogueBox) {
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

	// Sort type none for proper layering of images
	this->spriteBatch.begin(Bengine::GlyphSortType::NONE);

	if (this->currentState == ProgramState::FILESELECT) this->drawFileSelectScreen();
	else if (this->currentState == ProgramState::MAINSCREEN) this->drawMainScreen();
	else if (this->currentState == ProgramState::ADDSCENE || this->currentState == ProgramState::ADD_DIALOGUE) this->drawSceneCreationScreen();

	this->spriteBatch.end();
	this->spriteBatch.renderBatch();

	if (this->currentState == ProgramState::FILESELECT) this->drawFileSelectTexts();
	else if (this->currentState == ProgramState::ADDSCENE || this->currentState == ProgramState::ADD_DIALOGUE) this->drawSceneCreationScreenTexts();
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
	static const unsigned int background = Bengine::ResourceManager::getTexture("Textures/choosefile.png").id;
	static const unsigned int openFileBtn = Bengine::ResourceManager::getTexture("Textures/openfile.png").id;
	static const unsigned int editFileBtn = Bengine::ResourceManager::getTexture("Textures/editfile.png").id;
	static const unsigned int exitBtn = Bengine::ResourceManager::getTexture("Textures/exit.png").id;

	// Background
	this->spriteBatch.draw(
		this->mainBgDestRect,
		this->mainUvRect,
		background,
		0.0f,
		this->color
	);

	// 'Open File' -button
	this->spriteBatch.draw(
		this->openFileBtnDestRect,
		this->mainUvRect,
		openFileBtn,
		0.0f,
		this->color
	);

	// 'Edit File' -button
	if (this->currentFileName.length() > 0) {
		this->spriteBatch.draw(
			this->editFileBtnDestRect,
			this->mainUvRect,
			editFileBtn,
			0.0f,
			this->color
		);
	}

	// 'Exit' -button
	this->spriteBatch.draw(
		this->exitBtnDestRect,
		this->mainUvRect,
		exitBtn,
		0.0f,
		this->color
	);
}


void MainProgram::drawMainScreen()
{
	static const unsigned int main_background = Bengine::ResourceManager::getTexture("Textures/main-bg.png").id;
	static const unsigned int arrow = Bengine::ResourceManager::getTexture("Textures/arrow.png").id;
	static const unsigned int add_new_button = Bengine::ResourceManager::getTexture("Textures/addnew.png").id;
	static const unsigned int sceneBox = Bengine::ResourceManager::getTexture("Textures/scene-box.png").id;
	static const unsigned int arrowSide = Bengine::ResourceManager::getTexture("Textures/arrow-side.png").id;

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
	if ((this->currentSceneListIdx > 0 && this->selectedSceneIdx == -1) || (this->currentDialogueListIdx > 0 && this->selectedSceneIdx != -1)) {
		this->spriteBatch.draw(
			this->upArrowDestDect,
			this->upArrowUvRect,
			arrow,
			0.0f,
			this->color
		);
	}

	int dialoguesSize = 0;

	if (this->selectedSceneIdx != -1)
		dialoguesSize = this->sceneManager->getDialogues(this->selectedSceneIdx).size();

	// Down arrow
	if ((allScenes.size() > 5 && this->currentSceneListIdx < allScenes.size() - 5 && this->selectedSceneIdx == -1)
		|| (dialoguesSize > 5 && this->currentDialogueListIdx < dialoguesSize - 5 && this->selectedSceneIdx != -1)) {
		this->spriteBatch.draw(
			this->downArrowDestDect,
			this->mainUvRect,
			arrow,
			0.0f,
			this->color
		);
	}

	// Left arrow (back), only draw if no scene is selected
	if (this->selectedSceneIdx != -1) {
		this->spriteBatch.draw(
			this->arrowLeftDestRect,
			glm::vec4(0.0f, 0.0f, -1.0f, 1.0f),
			arrowSide,
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
		std::vector<Dialogue *> dialogues = this->getShownDialogues(this->sceneManager->getDialogues(this->selectedSceneIdx));
		glm::vec4 destRect = this->sceneBoxDestRect;

		for (unsigned i = 0; i < dialogues.size(); i++) {
			this->spriteBatch.draw(
				destRect,
				this->mainUvRect,
				sceneBox,
				0.0f,
				this->color
			);
			destRect.y -= this->sceneBlockPositionDiff;
		}
	}
}


void MainProgram::drawCurrentDialogue()
{
	static const unsigned int blackBox = Bengine::ResourceManager::getTexture("Textures/schoolyard.png").id;

	static const unsigned int bgBtn = Bengine::ResourceManager::getTexture("Textures/bg-button.png").id;
	static const unsigned int char1Btn = Bengine::ResourceManager::getTexture("Textures/char1-button.png").id;
	static const unsigned int char2Btn = Bengine::ResourceManager::getTexture("Textures/char2-button.png").id;
	static const unsigned int saveBtn = Bengine::ResourceManager::getTexture("Textures/save-button.png").id;
	static const unsigned int deleteBtn = Bengine::ResourceManager::getTexture("Textures/delete-button.png").id;

	// Get the dialogues of the selected scene
	std::vector<Dialogue *> dialogues = this->sceneManager->getScenes()[this->selectedSceneIdx].second;

	/*std::cout << "Dialogues size: " << dialogues.size() << "\n";*/

	// Buttons for dialogue editing, only show if a dialogue is selected
	if (this->currentDialogue != nullptr) {
		this->spriteBatch.draw(
			this->bgBtnDestRect,
			this->mainUvRect,
			bgBtn,
			0.0f,
			this->color
		);

		this->spriteBatch.draw(
			this->char1BtnDestRect,
			this->mainUvRect,
			char1Btn,
			0.0f,
			this->color
		);

		this->spriteBatch.draw(
			this->saveBtnDestRect,
			this->mainUvRect,
			saveBtn,
			0.0f,
			this->color
		);

		this->spriteBatch.draw(
			this->deleteBtnDestRect,
			this->mainUvRect,
			deleteBtn,
			0.0f,
			this->color
		);

		this->spriteBatch.draw(
			this->char2BtnDestRect,
			this->mainUvRect,
			char2Btn,
			0.0f,
			this->color
		);

		// Background
		if (this->currentDialogue->background != "") {
			this->spriteBatch.draw(
				this->dialogueBgDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture(this->currentDialogue->background).id,
				0.0f,
				this->color
			);
		}

		// Left Character
		if (this->currentDialogue->left != "") {
			this->spriteBatch.draw(
				this->leftCharDestRect,
				this->flippedXUvRect,
				Bengine::ResourceManager::getTexture(this->currentDialogue->left).id,
				0.0f,
				this->color
			);
		}

		// Right character
		if (this->currentDialogue->right != "") {
			this->spriteBatch.draw(
				this->rightCharDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture(this->currentDialogue->right).id,
				0.0f,
				this->color
			);
		}

		// Talker box
		if (this->currentDialogue->background != "") {
			this->spriteBatch.draw(
				this->talkerBoxDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/TalkerBoxBlue.png").id,
				0.0f,
				this->color
			);
		}

		// Dialogue box
		if (this->currentDialogue->background != "") {
			this->spriteBatch.draw(
				this->textBoxDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/TextBoxBlueSolo.png").id,
				0.0f,
				this->color
			);
		}
	}
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
	if (this->currentFileName != "") {
		// Make the buffer that will hold the text
		char buffer[256];

		// Set the camera matrix
		GLint pLocation = this->shaderProgram.getUniformLocation("P");
		glm::mat4 cameraMatrix = this->fontCamera.getCameraMatrix();

		// Send the camera matrix
		glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

		this->fontBatch.begin();

		std::string fileName = this->currentFileName.substr(0, this->currentFileName.size() - 5);

		sprintf_s(buffer, "%s", fileName.c_str());

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

	// Scene names, only show if a scene isn't opened
	if (this->selectedSceneIdx == -1) {
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
	}
	// Dialogue names, show if a scene is selected
	else {
		std::vector<Dialogue *> dialogues = this->getShownDialogues(this->sceneManager->getDialogues(this->selectedSceneIdx));

		for (unsigned i = 0; i < dialogues.size(); i++) {
			// Fill the buffer with the text
			sprintf_s(buffer, "%s", dialogues[i]->name.c_str());

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
	}

	// Name of current dialogue
	if (this->currentDialogue != nullptr) {
		sprintf_s(buffer, "%s", this->currentDialogue->name.c_str());

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(this->screenWidth - 20, 540),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255),
			Bengine::Justification::RIGHT
		);
	}

	// 'Type talker name' -text
	if (this->currentDialogue != nullptr && this->currentDialogue->background != "" && this->clickedOnTalkerBox) {
		sprintf_s(buffer, "%s", "Type name");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(this->screenWidth / 2 - 20, 540),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255),
			Bengine::Justification::LEFT
		);
	}

	// Talker name
	if (this->currentDialogue != nullptr && this->currentDialogue->background != "") {
		sprintf_s(buffer, "%s", this->currentDialogue->talking.c_str());

		glm::vec4 dim = this->getInputDimensions(this->talkerBoxDestRect);

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(dim.x + dim.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - 8),
			glm::vec2(0.5f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255),
			Bengine::Justification::MIDDLE
		);
	}

	if (this->currentDialogue != nullptr && this->currentDialogue->background != "") {
		sprintf_s(buffer, "%s", this->currentDialogue->message.c_str());

		glm::vec4 dim = this->getInputDimensions(this->textBoxDestRect);

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(dim.x + 24, abs((dim.y + dim.a / 2) - this->screenHeight) + 46),
			glm::vec2(0.6f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);
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
	if (this->currentState == ProgramState::ADDSCENE) {
		sprintf_s(buffer, "%s", this->currentSceneName.c_str());
	}
	else if (this->currentState == ProgramState::ADD_DIALOGUE) {
		sprintf_s(buffer, "%s", this->currentDialogueName.c_str());
	}

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
		// Get 5 items from the map
		int counter = 0;

		for (auto it = allScenes.begin(); it != allScenes.end(); it++) {
			if (counter == this->currentSceneListIdx) {
				// Put the 5 scenes that will be shown into the 'shownScenes' vector
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


std::vector<Dialogue *> MainProgram::getShownDialogues(std::vector<Dialogue *> allDialogues)
{
	std::vector<Dialogue *> shownDialogues;
	std::vector<int> indexes;

	// If there's more dialogues that can be shown, only pick 5
	if (allDialogues.size() > 5) {
		// Get 5 items from the vector
		int counter = 0;

		for (unsigned j = 0; j < allDialogues.size(); j++) {
			if (counter == this->currentDialogueListIdx) {
				// Put the 5 dialogues that will be shown into the 'shownDialogues' vector
				for (unsigned i = 0; i < 5; i++) {
					shownDialogues.push_back(allDialogues[j]);
					indexes.push_back(j);

					j++;
				}

				break;
			}

			counter++;
		}
	}

	this->shownDialogueIndexes.clear();

	if (shownDialogues.size() == 0) {
		// Store shown scene indexes
		for (unsigned i = 0; i < allDialogues.size(); i++) {
			this->shownDialogueIndexes.push_back(i);
		}

		return allDialogues;
	}
	else {
		// Store shown scene indexes
		this->shownDialogueIndexes = indexes;

		return shownDialogues;
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

void MainProgram::submitDialogue()
{
	// Create a new dialogue
	this->sceneManager->addDialogue(
		this->selectedSceneIdx,
		new Dialogue(
			-1,
			this->currentDialogueName,
			"",
			"",
			"",
			"",
			"",
			-1
		)
	);

	std::vector<Dialogue *> dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);

	this->currentDialogue = dialogues[dialogues.size() - 1];
}


std::wstring MainProgram::getOpenFileName(HWND owner, bool png)
{
	// Save current working directory
	TCHAR currentDirectory[MAX_PATH];
	DWORD ret;
	ret = GetCurrentDirectory(ARRSIZE(currentDirectory), currentDirectory);

	if (ret == 0) {
		std::cout << "Error with saving directory: " << GetLastError() << "\n";
	}
	else if (ret > ARRSIZE(currentDirectory)) {
		std::cout << "Error: buffer was too small when saving current directory!\n";
	}

	wchar_t buffer[MAX_PATH];

	OPENFILENAMEW ofn = { 0 };

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner;

	// Show only the appropriate file type
	if (png) ofn.lpstrFilter = L"PNG Files\0*.png\0";
	else ofn.lpstrFilter = L"Yaml Files\0*.yaml\0";

	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = L"Select an image";
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

	// Need to set the first character to null to avoid crashes
	buffer[0] = '\0';

	if (!GetOpenFileNameW(&ofn))
		return L"";

	// Set the current directory back to the root of the program so that images will load correctly
	if (!SetCurrentDirectory(currentDirectory)) {
		std::cout << "Error: could not set current directory back to default!\n";
	}

	return buffer;
}