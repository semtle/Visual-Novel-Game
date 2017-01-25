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
			if (this->currentFileName != "") this->sceneManager->saveScenes("../Visual Novel Game/Dialogues/" + this->currentFileName);
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
				for (wchar_t c : wFilePath) {
					filePath += static_cast<char>(c);;
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

	glm::vec4 dim;

	// 'Set Next Dialogue' -button
	// Do this here because need to check before clicks on answe boxes update
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->question
		&& (this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox) && !this->settingNextDialogue) {

		dim = this->getInputDimensions(this->setNextBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->settingNextDialogue = true;

					if (this->clickedOnFirstAnswerBox) this->selectedAnswerBox = 1;
					else if (this->clickedOnSecondAnswerBox) this->selectedAnswerBox = 2;
					else this->selectedAnswerBox = 3;

					this->lastSceneIdx = this->selectedSceneIdx;
					this->lastDialogueIdx = this->selectedDialogueIdx;
					this->selectedDialogueIdx = -1;
					
					this->lastDialogue = this->currentDialogue;

					this->currentDialogue = nullptr;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->talkerBoxDestRect);

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

	dim = this->getInputDimensions(this->firstAnswerBoxDestRect);

	// Check if mouse clicked outside of first answer box
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->clickedOnFirstAnswerBox = false;
		}
	}

	dim = this->getInputDimensions(this->secondAnswerBoxDestRect);

	// Check if mouse clicked outside of second answer box
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->clickedOnSecondAnswerBox = false;
		}
	}

	dim = this->getInputDimensions(this->thirdAnswerBoxDestRect);

	// Check if mouse clicked outside of third answer box
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->clickedOnThirdAnswerBox = false;
		}
	}

	dim = this->getInputDimensions(this->addNewDestRect);
	
	// Mouse is inside the 'add new' button
	if (!this->changingSettings && !this->settingNextDialogue) {
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
	}

	dim = this->getInputDimensions(this->upArrowDestDect);

	// Mouse is inside the up arrow
	if (!this->changingSettings) {
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
	}

	dim = this->getInputDimensions(this->downArrowDestDect);

	int dialoguesSize = 0;
	if (this->selectedSceneIdx != -1)
		dialoguesSize = this->sceneManager->getDialogues(this->selectedSceneIdx).size();

	// Mouse is inside the down arrow
	if (!this->changingSettings) {
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
						}
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
		if (!this->changingSettings) {
			// Check if any of the dialogue boxes were clicked on
			for (unsigned i = 0; i < this->showDialogueBoxPositions.size(); i++) {
				dim = this->getInputDimensions(glm::vec4(this->showDialogueBoxPositions[i].second, this->SCENE_BOX_WIDTH, this->SCENE_BOX_HEIGHT));

				if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
					if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
						if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
							this->inputManager.releaseKey(SDL_BUTTON_LEFT);

							// Open the clicked scene
							this->selectedDialogueIdx = this->shownDialogueIndexes[i];

							// Set the current dialogue
							if (!this->settingNextDialogue)
								this->currentDialogue = this->sceneManager->getDialogues(this->selectedSceneIdx)[this->selectedDialogueIdx];
						}
					}
				}
			}
		}
	}

	dim = getInputDimensions(this->arrowLeftDestRect);

	// Check if the back arrow was clicked on
	if (!this->changingSettings) {
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
	}

	// Cancel Next Dialogue button
	if (this->settingNextDialogue) {
		dim = this->getInputDimensions(this->cancelNextDlgBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->settingNextDialogue = false;
					this->selectedAnswerBox = -1;
					if (this->customNextDlg) this->changingSettings = true;
					else this->resetCurrentDialogue();
					this->customNextDlg = false;
				}
			}
		}
	}

	// Background button
	if (this->currentDialogue != nullptr && !this->settingNextDialogue) {
		dim = this->getInputDimensions(this->bgBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					// Open up the file explorer to pick a background image
					std::wstring wFilePath = this->getOpenFileName(NULL);

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (wchar_t c : wFilePath) {
						filePath += static_cast<char>(c);
					}

					// Set the background
					if (filePath.length() > 0) {
						std::string fileName = filePath.substr(filePath.find("Backgrounds") + 12, filePath.length());

						std::vector<std::string> sceneBackgrounds = this->sceneManager->getSceneBackgrounds();

						if (sceneBackgrounds.size() > this->selectedSceneIdx && sceneBackgrounds.size() > 0) {
							sceneBackgrounds[this->selectedSceneIdx] = fileName;
						}
						else {
							sceneBackgrounds.push_back(fileName);
						}

						this->sceneManager->setSceneBackgrounds(sceneBackgrounds);
					}
				}
			}
		}
	}

	// Char 1 button
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && !this->currentDialogue->question && !this->settingNextDialogue) {
		dim = this->getInputDimensions(this->char1BtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					// Open up the file explorer to pick a background image
					std::wstring wFilePath = this->getOpenFileName(NULL);

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (wchar_t c : wFilePath) {
						filePath += static_cast<char>(c);
					}

					size_t pos = filePath.find("Characters");

					std::string endPart = filePath.substr(pos + 11, filePath.length());
					size_t dash = endPart.find("\\");

					std::string charName = endPart.substr(0, dash);
					std::string imageName = endPart.substr(dash + 1, endPart.length() - 4);

					// Set the background
					if (filePath.length() > 0) {
						this->currentDialogue->left = charName + ", " + imageName;
					}
				}
			}
		}
	}

	// Char 2 button
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && !this->currentDialogue->question && !this->settingNextDialogue) {
		dim = this->getInputDimensions(this->char2BtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					// Open up the file explorer to pick a background image
					std::wstring wFilePath = this->getOpenFileName(NULL);

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (wchar_t c : wFilePath) {
						filePath += static_cast<char>(c);
					}

					size_t pos = filePath.find("Characters");

					std::string endPart = filePath.substr(pos + 11, filePath.length());
					size_t dash = endPart.find("\\");

					std::string charName = endPart.substr(0, dash);
					std::string imageName = endPart.substr(dash + 1, endPart.length() - 4);

					// Set the background
					if (filePath.length() > 0) {
						this->currentDialogue->right = charName + ", " + imageName;
					}
				}
			}
		}
	}

	// Settings button
	if (this->currentDialogue != nullptr && !this->settingNextDialogue) {
		dim = this->getInputDimensions(this->settingsBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->changingSettings = true;

					this->lastDialogue = this->currentDialogue;
					this->currentDialogue = nullptr;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->talkerBoxDestRect);

	// Talker box
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->talker) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnTalkerBox = true;

					// Don't allow to click both talker box and dialogue box at the same time
					if (this->clickedOnDialogueBox) this->clickedOnDialogueBox = false;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->textBoxDestRect);

	// Dialogue box
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->showTextBox) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnDialogueBox = true;

					// Don't allow to click both talker box and dialogue box at the same time
					if (this->clickedOnTalkerBox) this->clickedOnTalkerBox = false;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->firstAnswerBoxDestRect);

	// First answer box
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->question && !this->settingNextDialogue) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnFirstAnswerBox = true;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->secondAnswerBoxDestRect);

	// Second answer box
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->question && !this->settingNextDialogue) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnSecondAnswerBox = true;
				}
			}
		}
	}

	dim = this->getInputDimensions(this->thirdAnswerBoxDestRect);

	// Third answer box
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->question && !this->settingNextDialogue) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->clickedOnThirdAnswerBox = true;
				}
			}
		}
	}

	// Close icon
	if (this->changingSettings) {
		dim = this->getInputDimensions(this->closeIconDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->changingSettings = false;
					this->currentDialogue = this->lastDialogue;
				}
			}
		}
	}

	// Show dialogue box checkbox
	if (this->changingSettings) {
		dim = this->getInputDimensions(this->showDlgCheckBox);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->lastDialogue->showTextBox = !this->lastDialogue->showTextBox;
					
					if (!this->lastDialogue->showTextBox) {
						this->lastDialogue->talker = false;
					} else {
						this->lastDialogue->question = false;
					}
				}
			}
		}
	}

	// Show talker box checkbox
	if (this->changingSettings) {
		dim = this->getInputDimensions(this->talkerCheckBox);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->lastDialogue->talker = !this->lastDialogue->talker;

					if (this->lastDialogue->talker) {
						this->lastDialogue->showTextBox = true;
						this->lastDialogue->question = false;
					}
				}
			}
		}
	}

	// Question checkbox
	if (this->changingSettings) {
		dim = this->getInputDimensions(this->questionCheckBox);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->lastDialogue->question = !this->lastDialogue->question;

					if (this->lastDialogue->question) {
						this->lastDialogue->showTextBox = false;
						this->lastDialogue->talker = false;
						this->lastDialogue->nextDialogue = "";
					}
				}
			}
		}
	}

	// Custom next dialogue checkbox
	if (this->changingSettings) {
		dim = this->getInputDimensions(this->customNextCheckBox);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->lastDialogue->question = false;

					this->lastDialogue->nextDialogue = (this->lastDialogue->nextDialogue == "") ? "Next Dialogue Not Set" : "";
				}
			}
		}
	}

	// Custom next dialogue button
	if (this->changingSettings && this->lastDialogue->nextDialogue != "") {
		dim = this->getInputDimensions(this->setNextCustomDlgDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->settingNextDialogue = true;
					this->customNextDlg = true;

					this->lastSceneIdx = this->selectedSceneIdx;
					this->lastDialogueIdx = this->selectedDialogueIdx;
					this->selectedDialogueIdx = -1;

					this->changingSettings = false;
					std::cout << "Is currentdlg nullptr: " << std::to_string(this->currentDialogue == nullptr) << "\n";
				}
			}
		}
	}

	if (this->dlgButtonsOnRight && this->currentDialogue != nullptr && !this->currentDialogue->question) {
		this->saveBtnDestRect.x = -this->screenWidth / 2 + 335;
		this->settingsBtnDestRect.x = -this->screenWidth / 2 + 450;
		this->deleteBtnDestRect.x = -this->screenWidth / 2 + 565;

		this->dlgButtonsOnRight = false;
	}
	else if (!this->dlgButtonsOnRight && this->currentDialogue != nullptr && this->currentDialogue->question) {
		this->saveBtnDestRect.x = -this->screenWidth / 2 + 455;
		this->settingsBtnDestRect.x = -this->screenWidth / 2 + 570;
		this->deleteBtnDestRect.x = -this->screenWidth / 2 + 690;

		this->dlgButtonsOnRight = true;
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
	if (this->currentState == ProgramState::ADD_DIALOGUE || this->currentState == ProgramState::ADDSCENE
		|| this->clickedOnTalkerBox || this->clickedOnDialogueBox || this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox
		|| this->settingNextDialogue
		) {
		// Handle modifying the current scene name being given
		std::string keyName = SDL_GetKeyName(keyID);
		static std::string allowedKeys[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "-", "Space", ".", ",", "'", "+", "1", "Backspace", "Return", "Up", "Down"};

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
			// First answer box
			else if (this->currentDialogue != nullptr && this->clickedOnFirstAnswerBox) {
				this->currentDialogue->option1Text = this->currentDialogue->option1Text.substr(0, this->currentDialogue->option1Text.size() - 1);
			}
			// Second answer box
			else if (this->currentDialogue != nullptr && this->clickedOnSecondAnswerBox) {
				this->currentDialogue->option2Text = this->currentDialogue->option2Text.substr(0, this->currentDialogue->option2Text.size() - 1);
			}
			// Third answer box
			else if (this->currentDialogue != nullptr && this->clickedOnThirdAnswerBox) {
				this->currentDialogue->option3Text = this->currentDialogue->option3Text.substr(0, this->currentDialogue->option3Text.size() - 1);
			}
			// Dialogue name
			else {
				this->currentDialogueName = this->currentDialogueName.substr(0, this->currentDialogueName.size() - 1);
			}
		}

		// Add a space to the text
		// Scene name
		if (keyName == "Space") {
			if (this->currentState == ProgramState::ADDSCENE) {
				if (this->currentSceneName.length() < 16) {
					if (this->currentSceneName.length() > 0) this->currentSceneName += " ";
				}
			}

			// Dialogue name
			else if (this->currentState == ProgramState::ADD_DIALOGUE) {
				if (this->currentDialogueName.length() < 16) {
					if (this->currentDialogueName.length() > 0) this->currentDialogueName += " ";
				}
			}

			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
				this->currentDialogue->message += " ";
			}

			// First answer box
			else if (this->currentDialogue != nullptr && this->clickedOnFirstAnswerBox) {
				this->currentDialogue->option1Text += " ";
			}

			// Second answer box
			else if (this->currentDialogue != nullptr && this->clickedOnSecondAnswerBox) {
				this->currentDialogue->option2Text += " ";
			}

			// Third answer box
			else if (this->currentDialogue != nullptr && this->clickedOnThirdAnswerBox) {
				this->currentDialogue->option3Text += " ";
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
			// First answer box
			else if (this->currentDialogue != nullptr && this->clickedOnFirstAnswerBox) {
				this->clickedOnFirstAnswerBox = false;
				this->settingNextDialogue = false;
				this->selectedAnswerBox = -1;
				this->resetCurrentDialogue();
			}
			// Second answer box
			else if (this->currentDialogue != nullptr && this->clickedOnSecondAnswerBox) {
				this->clickedOnSecondAnswerBox = false;
				this->settingNextDialogue = false;
				this->selectedAnswerBox = -1;
				this->resetCurrentDialogue();
			}
			// Third answer box
			else if (this->currentDialogue != nullptr && this->clickedOnThirdAnswerBox) {
				this->clickedOnThirdAnswerBox = false;
				this->settingNextDialogue = false;
				this->selectedAnswerBox = -1;
				this->resetCurrentDialogue();
			}
			// Setting dialogue
			else if (this->settingNextDialogue) {
				if (this->selectedDialogueIdx != -1) {
					std::string nextScene = this->sceneManager->getScenes()[this->selectedSceneIdx].first;
					std::string nextDlg = this->sceneManager->getDialogues(this->selectedSceneIdx)[this->selectedDialogueIdx]->name;
					std::string next = this->currentFileName + "," + nextScene + "," + nextDlg;

					if (this->customNextDlg) {
						this->changingSettings = true;
						this->currentDialogue = nullptr;

						// Reset the scene & dialogue
						this->selectedSceneIdx = this->lastSceneIdx;
						this->selectedDialogueIdx = this->lastDialogueIdx;
					}
					else this->resetCurrentDialogue();
					
					if (this->selectedAnswerBox == 1) this->currentDialogue->option1Next = next;
					else if (this->selectedAnswerBox == 2) this->currentDialogue->option2Next = next;
					else if (this->selectedAnswerBox == 3) this->currentDialogue->option3Next = next;
					else this->lastDialogue->nextDialogue = next;

					this->customNextDlg = false;

					this->selectedAnswerBox = -1;
					this->settingNextDialogue = false;
				}
			}
			// Dialogue name
			else {
				if (this->currentDialogueName.length() >= 3 && this->currentDialogueName.length() <= 16) {
					std::cout << "Name was valid.\n";
				}
			}
		}

		// Add the character to the player's name
		if (keyName != "Return" && keyName != "Space" && keyName != "Backspace" && keyName != "Up" && keyName != "Down") {
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
			else if (this->currentDialogue != nullptr && (this->clickedOnDialogueBox || this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox)) {
				// Question mark
				if (keyName == "+" && (this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) {
					if (this->clickedOnDialogueBox) this->currentDialogue->message += "?";
					else if (this->clickedOnFirstAnswerBox) this->currentDialogue->option1Text += "?";
					else if (this->clickedOnSecondAnswerBox) this->currentDialogue->option2Text += "?";
					else if (this->clickedOnThirdAnswerBox) this->currentDialogue->option3Text += "?";
				}
				// Exclamation mark
				else if (keyName == "1" && (this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) {
					if (this->clickedOnDialogueBox) this->currentDialogue->message += "!";
					else if (this->clickedOnFirstAnswerBox) this->currentDialogue->option1Text += "!";
					else if (this->clickedOnSecondAnswerBox) this->currentDialogue->option2Text += "!";
					else if (this->clickedOnThirdAnswerBox) this->currentDialogue->option3Text += "!";
				}
				// Upper case letter (while holding shift)
				else {
					char letter = ((this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) ? toupper(static_cast<char>(keyID)) : static_cast<char>(keyID);

					if (this->clickedOnDialogueBox) this->currentDialogue->message += letter;
					else if (this->clickedOnFirstAnswerBox) this->currentDialogue->option1Text += letter;
					else if (this->clickedOnSecondAnswerBox) this->currentDialogue->option2Text += letter;
					else if (this->clickedOnThirdAnswerBox) this->currentDialogue->option3Text += letter;
				}
			}
			// Dialogue name
			else {
				if (this->currentDialogueName.length() < 16 && keyName != "+" && keyName != "1" && keyName != ",") {
					this->currentDialogueName += static_cast<char>(keyID);
				}
			}
		}

		// Add influence to the selected answer
		if (this->currentDialogue != nullptr && keyName == "Up") {
			if (this->clickedOnFirstAnswerBox) {
				this->currentDialogue->option1Influence += (this->currentDialogue->option1Influence < 2) ? 1 : 0;
			}
			else if (this->clickedOnSecondAnswerBox) {
				this->currentDialogue->option2Influence += (this->currentDialogue->option2Influence < 2) ? 1 : 0;
			}
			else if (this->clickedOnThirdAnswerBox) {
				this->currentDialogue->option3Influence += (this->currentDialogue->option3Influence < 2) ? 1 : 0;
			}
		}

		// Remove influece from the selected answer
		if (this->currentDialogue != nullptr && keyName == "Down") {
			if (this->clickedOnFirstAnswerBox) {
				this->currentDialogue->option1Influence -= (this->currentDialogue->option1Influence > 0) ? 1 : 0;
			}
			else if (this->clickedOnSecondAnswerBox) {
				this->currentDialogue->option2Influence -= (this->currentDialogue->option2Influence > 0) ? 1 : 0;
			}
			else if (this->clickedOnThirdAnswerBox) {
				this->currentDialogue->option3Influence -= (this->currentDialogue->option3Influence > 0) ? 1 : 0;
			}
		}

		// Capitalize first letter of the text
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
	if (!this->changingSettings && !this->settingNextDialogue) {
		this->spriteBatch.draw(
			this->addNewDestRect,
			this->mainUvRect,
			add_new_button,
			0.0f,
			this->color
		);
	}

	// Cancel button when selected 'next dialogue'
	if (this->settingNextDialogue) {
		this->spriteBatch.draw(
			this->cancelNextDlgBtnDestRect,
			this->mainUvRect,
			Bengine::ResourceManager::getTexture("Textures/cancel-button.png").id,
			0.0f,
			this->color
		);
	}

	// Up arrow
	if (!this->changingSettings) {
		if ((this->currentSceneListIdx > 0 && this->selectedSceneIdx == -1) || (this->currentDialogueListIdx > 0 && this->selectedSceneIdx != -1)) {
			this->spriteBatch.draw(
				this->upArrowDestDect,
				this->upArrowUvRect,
				arrow,
				0.0f,
				this->color
			);
		}
	}

	int dialoguesSize = 0;

	if (this->selectedSceneIdx != -1)
		dialoguesSize = this->sceneManager->getDialogues(this->selectedSceneIdx).size();

	// Down arrow
	if (!this->changingSettings) {
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
	}

	// Left arrow (back), only draw if no scene is selected
	if (this->selectedSceneIdx != -1 && !this->changingSettings) {
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
		if (!this->changingSettings) {
			std::vector<Dialogue *> dialogues = this->getShownDialogues(this->sceneManager->getDialogues(this->selectedSceneIdx));
			glm::vec4 destRect = this->sceneBoxDestRect;

			static const unsigned int greenBox = Bengine::ResourceManager::getTexture("Textures/scene-box-green.png").id;

			for (unsigned i = 0; i < dialogues.size(); i++) {
				this->spriteBatch.draw(
					destRect,
					this->mainUvRect,
					(i == this->selectedDialogueIdx && this->settingNextDialogue) ? greenBox : sceneBox,
					0.0f,
					this->color
				);
				destRect.y -= this->sceneBlockPositionDiff;
			}
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

	// Buttons for dialogue editing, only show if a dialogue is selected
	if (this->currentDialogue != nullptr && !this->settingNextDialogue) {
		this->spriteBatch.draw(
			this->bgBtnDestRect,
			this->mainUvRect,
			bgBtn,
			0.0f,
			this->color
		);

		// 'Set Next Dialogue' button
		if (this->currentDialogue->question && (this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox)) {
			this->spriteBatch.draw(
				setNextBtnDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/setnext.png").id,
				0.0f,
				this->color
			);
		}

		// Char 1 button
		if (!this->currentDialogue->question) {
			this->spriteBatch.draw(
				this->char1BtnDestRect,
				this->mainUvRect,
				char1Btn,
				0.0f,
				this->color
			);

			// Char 2 button
			this->spriteBatch.draw(
				this->char2BtnDestRect,
				this->mainUvRect,
				char2Btn,
				0.0f,
				this->color
			);
		}

		// Settings button
		this->spriteBatch.draw(
			this->settingsBtnDestRect,
			this->mainUvRect,
			Bengine::ResourceManager::getTexture("Textures/settings.png").id,
			0.0f,
			this->color
		);

		// Save button
		this->spriteBatch.draw(
			this->saveBtnDestRect,
			this->mainUvRect,
			saveBtn,
			0.0f,
			this->color
		);

		// Delete button
		this->spriteBatch.draw(
			this->deleteBtnDestRect,
			this->mainUvRect,
			deleteBtn,
			0.0f,
			this->color
		);

		// Background
		if (this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx) {
			this->spriteBatch.draw(
				this->dialogueBgDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("../Visual Novel Game/Textures/Backgrounds/" + this->sceneManager->getSceneBackgrounds()[this->selectedSceneIdx]).id,
				0.0f,
				this->color
			);
		}

		std::string lc = this->currentDialogue->left;

		// Left Character
		if (this->currentDialogue->left != "" && !this->currentDialogue->question) {
			this->spriteBatch.draw(
				this->leftCharDestRect,
				this->flippedXUvRect,
				Bengine::ResourceManager::getTexture("../Visual Novel Game/Textures/Characters/" + lc.substr(0, lc.find(",")) + "/" + lc.substr(lc.find(",") + 2, lc.length())).id,
				0.0f,
				this->color
			);
		}

		std::string rc = this->currentDialogue->right;

		// Right character
		if (this->currentDialogue->right != "" && !this->currentDialogue->question) {
			this->spriteBatch.draw(
				this->rightCharDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("../Visual Novel Game/Textures/Characters/" + rc.substr(0, rc.find(",")) + "/" + rc.substr(rc.find(",") + 2, rc.length())).id,
				0.0f,
				this->color
			);
		}

		// Talker box
		if (this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->talker) {
			this->spriteBatch.draw(
				this->talkerBoxDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/TalkerBoxBlue.png").id,
				0.0f,
				this->color
			);
		}

		// Dialogue box
		if (this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->showTextBox) {
			this->spriteBatch.draw(
				this->textBoxDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/TextBoxBlueSolo.png").id,
				0.0f,
				this->color
			);
		}

		/* Question boxes */
		if (this->currentDialogue->question && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx) {
			unsigned int firstBox, secondBox, thirdBox;

			// Determine first box color
			if (this->currentDialogue->option1Influence == 0) {
				firstBox = Bengine::ResourceManager::getTexture("Textures/RedOptionBox.png").id;
			}
			else if (this->currentDialogue->option1Influence == 1) {
				firstBox = Bengine::ResourceManager::getTexture("Textures/BlueOptionBox.png").id;
			}
			else {
				firstBox = Bengine::ResourceManager::getTexture("Textures/GreenOptionBox.png").id;
			}

			// Determine second box color
			if (this->currentDialogue->option2Influence == 0) {
				secondBox = Bengine::ResourceManager::getTexture("Textures/RedOptionBox.png").id;
			}
			else if (this->currentDialogue->option2Influence == 1) {
				secondBox = Bengine::ResourceManager::getTexture("Textures/BlueOptionBox.png").id;
			}
			else {
				secondBox = Bengine::ResourceManager::getTexture("Textures/GreenOptionBox.png").id;
			}

			// Determine second box color
			if (this->currentDialogue->option3Influence == 0) {
				thirdBox = Bengine::ResourceManager::getTexture("Textures/RedOptionBox.png").id;
			}
			else if (this->currentDialogue->option3Influence == 1) {
				thirdBox = Bengine::ResourceManager::getTexture("Textures/BlueOptionBox.png").id;
			}
			else {
				thirdBox = Bengine::ResourceManager::getTexture("Textures/GreenOptionBox.png").id;
			}

			// First answer box
			this->spriteBatch.draw(
				this->firstAnswerBoxDestRect,
				this->mainUvRect,
				firstBox,
				0.0f,
				this->color
			);

			// Second answer box
			this->spriteBatch.draw(
				this->secondAnswerBoxDestRect,
				this->mainUvRect,
				secondBox,
				0.0f,
				this->color
			);

			// First answer box
			this->spriteBatch.draw(
				this->thirdAnswerBoxDestRect,
				this->mainUvRect,
				thirdBox,
				0.0f,
				this->color
			);
		}

		if (this->currentDialogue->question && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx) {
			// Black boxes so can see next dialogue text
			glm::vec4 dim = this->firstAnswerBoxDestRect;

			this->spriteBatch.draw(
				glm::vec4(dim.x, dim.y + 85, 350, 30),
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/blackbox.png").id,
				0.0f,
				this->color
			);

			dim = this->secondAnswerBoxDestRect;

			this->spriteBatch.draw(
				glm::vec4(dim.x, dim.y + 85, 350, 30),
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/blackbox.png").id,
				0.0f,
				this->color
			);

			dim = this->thirdAnswerBoxDestRect;

			this->spriteBatch.draw(
				glm::vec4(dim.x, dim.y + 85, 350, 30),
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/blackbox.png").id,
				0.0f,
				this->color
			);
		}
	}
    else if (this->changingSettings) {
		unsigned int texture;

		// Show dialogue box
		if (this->lastDialogue->showTextBox) {
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
		}
		else {
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;
		}

		this->spriteBatch.draw(
			this->showDlgCheckBox,
			this->mainUvRect,
			texture,
			0.0f,
			this->color
		);

		// Talker
		if (this->lastDialogue->talker)
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
		else
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

		this->spriteBatch.draw(
			this->talkerCheckBox,
			this->mainUvRect,
			texture,
			0.0f,
			this->color
		);

		// Show dialogue box
		if (this->lastDialogue->question)
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
		else
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

		this->spriteBatch.draw(
			this->questionCheckBox,
			this->mainUvRect,
			texture,
			0.0f,
			this->color
		);

		// Custom next dialogue box
		if (this->lastDialogue->nextDialogue != "")
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
		else
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

		this->spriteBatch.draw(
			this->customNextCheckBox,
			this->mainUvRect,
			texture,
			0.0f,
			this->color
		);

		// Set next dialogue button
		if (this->lastDialogue->nextDialogue != "") {
			this->spriteBatch.draw(
				this->setNextCustomDlgDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/setnext.png").id,
				0.0f,
				this->color
			);
		}

		// Close icon
		this->spriteBatch.draw(
			this->closeIconDestRect,
			this->mainUvRect,
			Bengine::ResourceManager::getTexture("Textures/close_icon.png").id,
			0.0f,
			this->color
		);
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
		if (!this->changingSettings) {
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
	}

	// Name of current dialogue
	if (this->currentDialogue != nullptr && !this->settingNextDialogue && !this->settingNextDialogue) {
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

	// Select dialogue guide text
	if (this->settingNextDialogue) {
		sprintf_s(buffer, "%s", "Browse and click on a dialogue on the left.");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(220, 540),
			glm::vec2(0.7f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		sprintf_s(buffer, "%s", "Confirm selected dialogue with enter.");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(220, 510),
			glm::vec2(0.7f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		if (this->selectedDialogueIdx != -1) {
			sprintf_s(buffer, "%s: %s", "Selected dialogue", this->sceneManager->getDialogues(this->selectedSceneIdx)[this->selectedDialogueIdx]->name.c_str());
		}
		else {
			sprintf_s(buffer, "%s", "No dialogue selected.");
		}

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(220, 450),
			glm::vec2(0.7f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);
	}

	// 'Type talker name' -text
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->clickedOnTalkerBox) {
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

	// 'Type message' -text
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->clickedOnDialogueBox) {
		sprintf_s(buffer, "%s", "Type message");

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
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->talker) {
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

	// Dialogue message
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->showTextBox) {
		glm::vec4 dim = this->getInputDimensions(this->textBoxDestRect);
		const float fontScale = 0.6f;

		// Get wrapped text so the message doesn't go over the edge
		std::vector<std::string> wrappedMessage = this->getWrappedText(this->currentDialogue->message, this->spriteFont, 550, fontScale);

		for (unsigned i = 0; i < wrappedMessage.size(); i++) {
			sprintf_s(buffer, "%s", wrappedMessage[i].c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(dim.x + 24, abs((dim.y + dim.a / 2) - this->screenHeight) + 36 - (i * 26)),
				glm::vec2(fontScale),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);
		}
	}

	// Answer boxes
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->question && !this->settingNextDialogue) {
		// First box
		glm::vec4 dim = this->getInputDimensions(this->firstAnswerBoxDestRect);
		const float fontScale = 0.6f;

		// Get wrapped text so the message doesn't go over the edge
		std::vector<std::string> wrappedMessage = this->getWrappedText(this->currentDialogue->option1Text, this->spriteFont, 520, fontScale);

		if (wrappedMessage.size() == 1) {
			sprintf_s(buffer, "%s", wrappedMessage[0].c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(dim.x + this->firstAnswerBoxDestRect.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - (this->spriteFont->measure(wrappedMessage[0].c_str()).y * fontScale / 2) + 2),
				glm::vec2(fontScale),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255),
				Bengine::Justification::MIDDLE
			);
		}
		else {
			for (unsigned i = 0; i < wrappedMessage.size(); i++) {
				sprintf_s(buffer, "%s", wrappedMessage[i].c_str());

				this->spriteFont->draw(
					this->fontBatch,
					buffer,
					glm::vec2(dim.x + this->firstAnswerBoxDestRect.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - (i * 26)),
					glm::vec2(fontScale),
					0.0f,
					Bengine::ColorRGBA8(0, 0, 0, 255),
					Bengine::Justification::MIDDLE
				);
			}
		}

		// Second box
		dim = this->getInputDimensions(this->secondAnswerBoxDestRect);

		// Get wrapped text so the message doesn't go over the edge
		wrappedMessage = this->getWrappedText(this->currentDialogue->option2Text, this->spriteFont, 520, fontScale);

		if (wrappedMessage.size() == 1) {
			sprintf_s(buffer, "%s", wrappedMessage[0].c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(dim.x + this->secondAnswerBoxDestRect.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - (this->spriteFont->measure(wrappedMessage[0].c_str()).y * fontScale / 2) + 2),
				glm::vec2(fontScale),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255),
				Bengine::Justification::MIDDLE
			);
		}
		else {
			for (unsigned i = 0; i < wrappedMessage.size(); i++) {
				sprintf_s(buffer, "%s", wrappedMessage[i].c_str());

				this->spriteFont->draw(
					this->fontBatch,
					buffer,
					glm::vec2(dim.x + this->secondAnswerBoxDestRect.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - (i * 26)),
					glm::vec2(fontScale),
					0.0f,
					Bengine::ColorRGBA8(0, 0, 0, 255),
					Bengine::Justification::MIDDLE
				);
			}
		}

		// Third box
		dim = this->getInputDimensions(this->thirdAnswerBoxDestRect);

		// Get wrapped text so the message doesn't go over the edge
		wrappedMessage = this->getWrappedText(this->currentDialogue->option3Text, this->spriteFont, 520, fontScale);

		if (wrappedMessage.size() == 1) {
			sprintf_s(buffer, "%s", wrappedMessage[0].c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(dim.x + this->thirdAnswerBoxDestRect.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - (this->spriteFont->measure(wrappedMessage[0].c_str()).y * fontScale / 2) + 2),
				glm::vec2(fontScale),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255),
				Bengine::Justification::MIDDLE
			);
		}
		else {
			for (unsigned i = 0; i < wrappedMessage.size(); i++) {
				sprintf_s(buffer, "%s", wrappedMessage[i].c_str());

				this->spriteFont->draw(
					this->fontBatch,
					buffer,
					glm::vec2(dim.x + this->thirdAnswerBoxDestRect.z / 2, abs((dim.y + dim.a / 2) - this->screenHeight) - (i * 26)),
					glm::vec2(fontScale),
					0.0f,
					Bengine::ColorRGBA8(0, 0, 0, 255),
					Bengine::Justification::MIDDLE
				);
			}
		}
	}

	// Next dialogue texts for answers
	if (this->currentDialogue != nullptr && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->question && !this->settingNextDialogue) {
		const float fontScale = 0.7f;

		// Get only dialogue name
		std::string dlg = this->currentDialogue->option1Next;
		if (dlg != "Not Selected") dlg = dlg.substr(dlg.find_last_of(",") + 1, dlg.length());
		sprintf_s(buffer, "%s: %s", "Next Dialogue", dlg.c_str());

		glm::vec4 dim = this->getInputDimensions(this->firstAnswerBoxDestRect);

		// First answer
		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(dim.x + 5, abs(dim.y - this->screenHeight)),
			glm::vec2(fontScale),
			0.0f,
			this->color
		);

		// Get only dialogue name
		dlg = this->currentDialogue->option2Next;
		if (dlg != "Not Selected") dlg = dlg.substr(dlg.find_last_of(",") + 1, dlg.length());
		sprintf_s(buffer, "%s: %s", "Next Dialogue", dlg.c_str());

		dim = this->getInputDimensions(this->secondAnswerBoxDestRect);

		// Second answer
		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(dim.x + 5, abs(dim.y - this->screenHeight)),
			glm::vec2(fontScale),
			0.0f,
			this->color
		);

		// Get only dialogue name
		dlg = this->currentDialogue->option3Next;
		if (dlg != "Not Selected") dlg = dlg.substr(dlg.find_last_of(",") + 1, dlg.length());
		sprintf_s(buffer, "%s: %s", "Next Dialogue", dlg.c_str());

		dim = this->getInputDimensions(this->thirdAnswerBoxDestRect);

		// Third answer
		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(dim.x + 5, abs(dim.y - this->screenHeight)),
			glm::vec2(fontScale),
			0.0f,
			this->color
		);
	}

	if (this->changingSettings) {
		// Show dialogue box checkbox
		sprintf_s(buffer, "%s", "Show the dialogue box");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(226, 550),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		// Talker checkbox
		sprintf_s(buffer, "%s", "Show talker");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(226, 470),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		// Question checkbox
		sprintf_s(buffer, "%s", "Make this a question");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(226, 390),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		// Custom next dialogue checkbox
		sprintf_s(buffer, "%s", "Set custom next dialogue");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(226, 310),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		// The custom next dialogue's name
		if (this->lastDialogue->nextDialogue != "") {
			std::string endPart = this->lastDialogue->nextDialogue;

			if (this->lastDialogue->nextDialogue != "Next Dialogue Not Set") {
				endPart = endPart.substr(endPart.find(",") + 1, endPart.length());
				endPart = endPart.substr(endPart.find(",") + 1, endPart.length());
			}

			sprintf_s(buffer, "%s", endPart.c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(385, 222),
				glm::vec2(0.8f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);
		}
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


void MainProgram::resetCurrentDialogue()
{
	this->currentDialogue = this->lastDialogue;
	this->selectedSceneIdx = this->lastSceneIdx;
	this->selectedDialogueIdx = this->lastDialogueIdx;
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
			true,
			false,
			true,
			"",
			"",
			1,
			"Not Selected",
			"",
			1,
			"Not Selected",
			"",
			1,
			"Not Selected"
		)
	);

	std::vector<Dialogue *> dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);

	this->currentDialogue = dialogues[dialogues.size() - 1];
}


std::vector<std::string> MainProgram::getWrappedText(std::string text, Bengine::SpriteFont* spriteFont, const float& maxLength, const float& fontScale)
{
	std::vector<std::string> lines;

	while (spriteFont->measure(text.c_str()).x * fontScale > maxLength) {
		// The current line that will be added
		std::string crop = "";
		for (unsigned i = 0; i < text.length(); i++) {
			crop += text[i];

			// If the line doesn't fit the box, it means that we should end the current line
			if (spriteFont->measure(crop.c_str()).x * fontScale > maxLength) {
				char currChar = text[i];

				// Go back to the last space so we don't crop in the middle of a word
				while (currChar != ' ') {
					i--;
					currChar = text[i];
				}
				crop = crop.substr(0, i);
				lines.push_back(crop);
				text = text.substr(i + 1, text.length() - 1);
				break;
			}
		}
	}

	if (text != "") lines.push_back(text);

	return lines;
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