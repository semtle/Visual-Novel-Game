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
	this->spriteFont = new Bengine::SpriteFont("Fonts/cabin-condensed.regular.ttf", 32);

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

		while (this->currentState == ProgramState::DELETE_DIALOGUE) {
			this->camera.update();
			this->fontCamera.update();
			this->inputManager.update();

			this->processInput();
			this->checkDialogueDeletionInputs();

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
			if (this->currentFileName != "") this->sceneManager->saveToFile("../Visual Novel Game/Dialogues/" + this->currentFileName);
			this->currentState = ProgramState::EXIT;
			break; 
		case SDL_KEYDOWN:
			this->inputManager.pressKey(event.key.keysym.sym);
			this->onKeyPress(event.key.keysym.sym);

            if (event.key.keysym.sym == SDLK_RIGHT) {
                if (this->clickedOnDialogueBox && this->currentMessageIdx < this->currentDialogue->message.length()) {
                    this->currentMessageIdx++;
                }

                else if (this->clickedOnFirstAnswerBox && this->currentMessageIdx < this->currentDialogue->option1Text.length()) {
                    this->currentMessageIdx++;
                }

                else if (this->clickedOnSecondAnswerBox && this->currentMessageIdx < this->currentDialogue->option2Text.length()) {
                    this->currentMessageIdx++;
                }

                else if (this->clickedOnThirdAnswerBox && this->currentMessageIdx < this->currentDialogue->option3Text.length()) {
                    this->currentMessageIdx++;
                }
            }
            else if (event.key.keysym.sym == SDLK_LEFT) {
                if (this->currentMessageIdx > 0) this->currentMessageIdx--;
            }
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
		case SDL_MOUSEWHEEL:
			if (this->currentState != ProgramState::FILESELECT)
				this->onScroll(event.wheel.y);
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
				std::wstring wFilePath = this->getOpenFileName("dlg");

				// Convert the filepath from a wide string to a regural string
				std::string filePath = "";
				for (wchar_t c : wFilePath) {
					filePath += static_cast<char>(c);;
				}

                if (filePath.length() > 0) {
                    size_t pos = filePath.find("Dialogues");

                    this->currentFileName = filePath.substr(pos + 10);
                }
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

					this->sceneManager->loadFromFile("../Visual Novel Game/Dialogues/" + this->currentFileName);

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
					this->lastSceneListIdx = this->currentSceneListIdx;
					this->lastDialogueListIdx = this->currentDialogueListIdx;
					this->selectedDialogueIdx = -1;
					
					this->lastDialogue = this->currentDialogue;

					this->currentDialogue = nullptr;
					this->changingDialogueName = false;
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

	dim = this->getInputDimensions(this->saveBtnDestRect);

	// Clicked something else than save button
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->saved = false;
		}
	}

	dim = this->getInputDimensions(this->textBoxDestRect);

	// Check if mouse clicked outside of dialogue box
	if (mouseCoords.x < dim.x || mouseCoords.x > dim.x + dim.z || mouseCoords.y < dim.y || mouseCoords.y > dim.y + dim.a) {
		if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			this->clickedOnDialogueBox = false;
            this->currentMessageIdx = 0;
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

					this->changingSceneNameIdx = -1;
					this->deletingScene = false;

					if (this->selectedSceneIdx == -1) {
						this->currentState = ProgramState::ADDSCENE;
					}
					else {
                        this->lastDialogueIdx = this->selectedDialogueIdx;
                        this->lastDialogueListIdx = this->currentDialogueListIdx;
                        this->lastDialogue = this->currentDialogue;
						this->currentDialogue = nullptr;
						this->changingDialogueName = false;
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
					// Open scene
					if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
						this->inputManager.releaseKey(SDL_BUTTON_LEFT);

						// Open the clicked scene
						if (!this->settingNextDialogue) {
							this->selectedSceneIdx = this->shownSceneIndexes[i];
							this->changingSceneNameIdx = -1;
							this->deletingScene = false;
						}
						else {
							if (this->shownSceneIndexes[i] == this->lastSceneIdx) {
								this->selectedSceneIdx = this->shownSceneIndexes[i];
								this->selectedNextSceneIdx = -1;
							}
							else {
								this->selectedNextSceneIdx = this->shownSceneIndexes[i];
								this->selectedSceneIdx = -1;
							}
						}
					}
					// Change scene name
					else if (this->inputManager.isKeyDown(SDL_BUTTON_RIGHT) && !this->settingNextDialogue) {
						this->inputManager.releaseKey(SDL_BUTTON_RIGHT);

						this->changingSceneNameIdx = this->shownSceneIndexes[i];
						this->deletingScene = false;
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

	if (this->selectedSceneIdx == -1) {
		//if (this->inputManager.)
	}

	dim = getInputDimensions(this->arrowLeftDestRect);

	// Check if the back arrow was clicked on
	if (!this->changingSettings && !(this->settingNextDialogue && !this->customNextDlg)) {
		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					if (this->selectedSceneIdx == -1 && !this->settingNextDialogue) {
						this->resetEverything();
					}

					this->selectedSceneIdx = -1;
					this->selectedDialogueIdx = -1;
					this->changingSceneNameIdx = -1;
					this->deletingScene = false;
					this->currentDialogueListIdx = 0;

					this->currentDialogue = nullptr;
					this->changingDialogueName = false;
				}
			}
		}
	}

	// Deleting scene buttons
	if (this->deletingScene) {
		dim = this->getInputDimensions(this->deleteSceneBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->sceneManager->removeScene(this->changingSceneNameIdx);
					this->changingSceneNameIdx = -1;

					if (this->currentSceneListIdx > 0 && this->sceneManager->getScenes().size() < this->currentSceneListIdx + 5) {
						this->currentSceneListIdx--;
					}

					this->deletingScene = false;
				}
			}
		}

		dim = this->getInputDimensions(this->cancelSceneDeleteBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->deletingScene = false;
				}
			}
		}
	}

	// Current dialogue name
	if (this->currentDialogue != nullptr) {
		glm::vec2 fontSize = this->spriteFont->measure(this->currentDialogue->name.c_str());

		dim = this->getInputDimensions(glm::vec4(this->screenWidth / 2 - 20 - fontSize.x * 0.8, -this->screenHeight / 2 + 540, fontSize.x * 0.8, fontSize.y * 0.8));

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->changingDialogueName = true;
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
					this->selectedNextSceneIdx = -1;
					this->selectedAnswerBox = -1;
					if (this->customNextDlg) this->changingSettings = true;
					else this->resetCurrentDialogue();
					this->customNextDlg = false;
					this->selectedSceneIdx = this->lastSceneIdx;
					this->selectedDialogueIdx = this->lastDialogueIdx;

					this->currentSceneListIdx = this->lastSceneListIdx;
					this->currentDialogueListIdx = this->lastDialogueListIdx;
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
					std::wstring wFilePath = this->getOpenFileName("bg");

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
					std::wstring wFilePath = this->getOpenFileName("char");

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (wchar_t c : wFilePath) {
						filePath += static_cast<char>(c);
					}

                    if (filePath.length() > 0) {
					    size_t pos = filePath.find("Characters");

					    std::string endPart = filePath.substr(pos + 11, filePath.length());
					    size_t dash = endPart.find("\\");

					    std::string charName = endPart.substr(0, dash);
					    std::string imageName = endPart.substr(dash + 1, endPart.length() - 4);

					    // Set the character image
						this->currentDialogue->left = charName + ", " + imageName;
					}
				}
				else if (this->inputManager.isKeyDown(SDL_BUTTON_RIGHT)) {
					this->inputManager.releaseKey(SDL_BUTTON_RIGHT);
					this->currentDialogue->left = "";
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
					std::wstring wFilePath = this->getOpenFileName("char");

					// Convert the filepath from a wide string to a regural string
					std::string filePath = "";
					for (wchar_t c : wFilePath) {
						filePath += static_cast<char>(c);
					}

                    if (filePath.length() > 0) {
                        size_t pos = filePath.find("Characters");

                        std::string endPart = filePath.substr(pos + 11, filePath.length());
                        size_t dash = endPart.find("\\");

                        std::string charName = endPart.substr(0, dash);
                        std::string imageName = endPart.substr(dash + 1, endPart.length() - 4);

					    // Set the character image
						this->currentDialogue->right = charName + ", " + imageName;
					}
				}

				else if (this->inputManager.isKeyDown(SDL_BUTTON_RIGHT)) {
					this->inputManager.releaseKey(SDL_BUTTON_RIGHT);
					this->currentDialogue->right = "";
				}
			}
		}
	}

	// Save button
	if (this->currentDialogue != nullptr && !this->saved) {
		dim = this->getInputDimensions(this->saveBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->sceneManager->saveToFile("../Visual Novel Game/Dialogues/" + this->currentFileName);
					this->saved = true;
				}
			}
		}
	}

	// Delete button
	if (this->currentDialogue != nullptr) {
		dim = this->getInputDimensions(this->deleteBtnDestRect);

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->currentState = ProgramState::DELETE_DIALOGUE;
					this->lastDialogue = this->currentDialogue;
					this->currentDialogue = nullptr;
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
					this->changingDialogueName = false;
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
                    if (this->clickedOnDialogueBox) {
                        this->clickedOnDialogueBox = false;
                        this->currentMessageIdx = 0;
                    }
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
                    this->currentMessageIdx = this->currentDialogue->message.length();

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
                    this->currentMessageIdx = this->currentDialogue->option1Text.length();
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
                    this->currentMessageIdx = this->currentDialogue->option2Text.length();
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
                    this->currentMessageIdx = this->currentDialogue->option3Text.length();
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

    // Fade In checkbox
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->fadeInCheckBox);

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);
                    this->lastDialogue->fadeIn = !this->lastDialogue->fadeIn;
                }
            }
        }
    }

    // Fade Out checkbox
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->fadeOutCheckBox);

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);
                    this->lastDialogue->fadeOut = !this->lastDialogue->fadeOut;
                }
            }
        }
    }

	// Question checkbox
	if (this->changingSettings && this->lastDialogue->name.find("Question") != std::string::npos) {
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
		if (this->lastDialogue->name.find("Question") == std::string::npos) {
			dim.y -= 80;
		}

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
		if (this->lastDialogue->name.find("Question") == std::string::npos) {
			dim.y -= 80;
		}

		if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
			if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
				if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
					this->inputManager.releaseKey(SDL_BUTTON_LEFT);

					this->settingNextDialogue = true;
					this->customNextDlg = true;

					this->lastSceneIdx = this->selectedSceneIdx;
					this->lastDialogueIdx = this->selectedDialogueIdx;
					this->lastSceneListIdx = this->currentSceneListIdx;
					this->lastDialogueListIdx = this->currentDialogueListIdx;

					this->selectedDialogueIdx = -1;

					this->changingSettings = false;
					std::cout << "Is currentdlg nullptr: " << std::to_string(this->currentDialogue == nullptr) << "\n";
				}
			}
		}
	}

    // Start Music checkbox
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->startMusicCheckBox);
        if (this->lastDialogue->name.find("Question") == std::string::npos) {
            dim.y -= 80;
        }

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);

                    if (this->lastDialogue->startMusic != "") this->lastDialogue->startMusic = "";
                    else {
                        this->lastDialogue->startMusic = "Not Set";
                        this->lastDialogue->endMusic = false;
                    }

                    
                }
            }
        }
    }

    // End Music checkbox
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->endMusicCheckBox);
        if (this->lastDialogue->name.find("Question") == std::string::npos) {
            dim.y -= 80;
        }

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);

                    this->lastDialogue->endMusic = !this->lastDialogue->endMusic;
                    if (this->lastDialogue->endMusic) this->lastDialogue->startMusic = "";
                }
            }
        }
    }

    // Sound effect checkbox
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->soundEffectCheckBox);
        if (this->lastDialogue->name.find("Question") == std::string::npos) {
            dim.y -= 80;
        }

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);

                    if (this->lastDialogue->soundEffect != "") this->lastDialogue->soundEffect = "";
                    else this->lastDialogue->soundEffect = "Not Set";
                }
            }
        }
    }

    // Open music file
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->openMusicFileBtn);
        if (this->lastDialogue->name.find("Question") == std::string::npos) {
            dim.y -= 80;
        }

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);

                    // Open up the file explorer to pick a yaml file
                    std::wstring wFilePath = this->getOpenFileName("music");

                    // Convert the filepath from a wide string to a regural string
                    std::string filePath = "";
                    for (wchar_t c : wFilePath) {
                        filePath += static_cast<char>(c);;
                    }

                    if (filePath.length() > 0) {
                        size_t pos = filePath.find("Music");

                        this->lastDialogue->startMusic = filePath.substr(pos + 6);
                    }
                }
            }
        }
    }

    // Open sound effect file
    if (this->changingSettings) {
        dim = this->getInputDimensions(this->openSoundEffectFileBtn);
        if (this->lastDialogue->name.find("Question") == std::string::npos) {
            dim.y -= 80;
        }

        if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
            if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
                if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
                    this->inputManager.releaseKey(SDL_BUTTON_LEFT);

                    // Open up the file explorer to pick a yaml file
                    std::wstring wFilePath = this->getOpenFileName("soundeffect");

                    // Convert the filepath from a wide string to a regural string
                    std::string filePath = "";
                    for (wchar_t c : wFilePath) {
                        filePath += static_cast<char>(c);;
                    }

                    if (filePath.length() > 0) {
                        size_t pos = filePath.find("Sound Effects");

                        this->lastDialogue->soundEffect = filePath.substr(pos + 14);
                    }
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

				this->duplicateDialogueName = false;
				this->duplicateSceneName = false;
                this->creationErrorMsg = "";
				
				// Set the current dialogue to be the last one when cancelling
				if (this->selectedSceneIdx != -1) {
                    this->currentDialogue = this->lastDialogue;
                    this->currentDialogueListIdx = this->lastDialogueListIdx;
                    this->selectedDialogueIdx = this->lastDialogueIdx;
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
					auto scenes = this->sceneManager->getScenes();

					this->duplicateSceneName = false;

					for (unsigned i = 0; i < scenes.size(); i++) {
						if (scenes[i].first == this->currentSceneName) {
							this->duplicateSceneName = true;
							break;
						}
					}

					if (!this->duplicateSceneName) {
                        if (this->currentSceneName.find_first_not_of(' ') != std::string::npos) {
                            this->sceneManager->addScene(this->currentSceneName);

                            if (this->sceneManager->getScenes().size() > 5) {
                                this->currentSceneListIdx = this->sceneManager->getScenes().size() - 5;
                            }

                            this->currentState = ProgramState::MAINSCREEN;
                            this->creationErrorMsg = "";
                        }
                        else {
                            this->creationErrorMsg = "You have to set a name for your scene!";
                        }
					}
				}
				else if (this->currentState == ProgramState::ADD_DIALOGUE) {
					auto dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);

					this->duplicateDialogueName = false;

					for (unsigned i = 0; i < dialogues.size(); i++) {
						if (dialogues[i]->name == currentDialogueName) {
							this->duplicateDialogueName = true;
							break;
						}
					}

                    if (!this->duplicateDialogueName) {
                        if (this->currentDialogueName.find_first_not_of(' ') != std::string::npos) {
                            this->submitDialogue();
                            this->currentState = ProgramState::MAINSCREEN;
                            this->creationErrorMsg = "";
                        }
                        else {
                            this->creationErrorMsg = "You have to set a name for your dialogue!";
                        }
                    }
				}
			}
		}
	}

	// When pressing enter, submit the new scene
	if (this->inputManager.isKeyDown(SDLK_RETURN)) {
		this->inputManager.releaseKey(SDLK_RETURN);

		if (this->currentState == ProgramState::ADDSCENE) {
			auto scenes = this->sceneManager->getScenes();

			this->duplicateSceneName = false;

			for (unsigned i = 0; i < scenes.size(); i++) {
				if (scenes[i].first == this->currentSceneName) {
					this->duplicateSceneName = true;
					break;
				}
			}

			if (!this->duplicateSceneName) {
                if (this->currentSceneName.find_first_not_of(' ') != std::string::npos) {
                    this->sceneManager->addScene(this->currentSceneName);

                    if (this->sceneManager->getScenes().size() > 5) {
                        this->currentSceneListIdx = this->sceneManager->getScenes().size() - 5;
                    }

                    this->currentState = ProgramState::MAINSCREEN;
                    this->creationErrorMsg = "";
                } else {
                    this->creationErrorMsg = "You have to set a name for your scene!";
                }
			}
		}
		else if (this->currentState == ProgramState::ADD_DIALOGUE) {
			auto dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);

			this->duplicateDialogueName = false;

			for (unsigned i = 0; i < dialogues.size(); i++) {
				if (dialogues[i]->name == currentDialogueName) {
					this->duplicateDialogueName = true;
					break;
				}
			}

            if (!this->duplicateDialogueName) {
                if (this->currentDialogueName.find_first_not_of(' ') != std::string::npos) {
                    this->submitDialogue();
                    this->currentState = ProgramState::MAINSCREEN;
                    this->creationErrorMsg = "";
                }
                else {
                    this->creationErrorMsg = "You have to set a name for your dialogue!";
                }
            }
		}
	}
}


void MainProgram::checkDialogueDeletionInputs()
{
	glm::vec2 mouseCoords = this->inputManager.getMouseCoords();

	glm::vec4 dim = this->getInputDimensions(this->confirmDlgDeleteBtnDestRect);

	// Confirm delete
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				this->sceneManager->removeDialogue(this->selectedSceneIdx, this->selectedDialogueIdx);

				// Move down the list if removing dialogue from the end
				if (this->currentDialogueListIdx > 0 && this->sceneManager->getDialogues(this->selectedSceneIdx).size() < this->currentDialogueListIdx + 5) {
					this->currentDialogueListIdx--;
				}

				this->selectedDialogueIdx = -1;
				
				this->currentState = ProgramState::MAINSCREEN;
			}
		}
	}

	dim = this->getInputDimensions(this->cancelDlgDeleteBtnDestRect);

	// Cancel delete
	if (mouseCoords.x > dim.x && mouseCoords.x < dim.x + dim.z) {
		if (mouseCoords.y > dim.y && mouseCoords.y < dim.y + dim.a) {
			if (this->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager.releaseKey(SDL_BUTTON_LEFT);

				this->currentDialogue = this->lastDialogue;
				this->currentState = ProgramState::MAINSCREEN;
			}
		}
	}
}


void MainProgram::onKeyPress(unsigned int keyID)
{
	// This function handles writing text in many different places in the editor
	if (this->currentState == ProgramState::ADD_DIALOGUE || this->currentState == ProgramState::ADDSCENE
		|| this->clickedOnTalkerBox || this->clickedOnDialogueBox || this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox
		|| this->settingNextDialogue || this->changingDialogueName || this->changingSceneNameIdx != -1 ||this->selectedDialogueIdx != -1 || this->selectedSceneIdx != -1 || clickedOnDialogueBox
		) {
		// Handle modifying the current scene name being given
		std::string keyName = SDL_GetKeyName(keyID);
		static std::string allowedKeys[] = {
			"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
			"-", "Space", ".", ",", "'", "+", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Backspace", "Return", "Up", "Down", "Delete"
		};

		bool keyAllowed = false;

		for (std::string name : allowedKeys) {
			if (name == keyName) keyAllowed = true;
		}

		if (!keyAllowed) return;

		this->saved = false;

		// Remove the last character
		if (keyName == "Backspace") {
			// Scene name
			if (this->currentState == ProgramState::ADDSCENE) {
				this->currentSceneName = this->currentSceneName.substr(0, this->currentSceneName.size() - 1);
				this->duplicateSceneName = false;
                this->creationErrorMsg = "";
			}
			// Dialogue name
			if (this->currentState == ProgramState::ADD_DIALOGUE) {
				this->currentDialogueName = this->currentDialogueName.substr(0, this->currentDialogueName.size() - 1);
				this->duplicateDialogueName = false;
                this->creationErrorMsg = "";
			}
			// Changing dialogue name
			else if (this->changingDialogueName) {
				this->currentDialogue->name = this->currentDialogue->name.substr(0, this->currentDialogue->name.size() - 1);
			}
			else if (this->changingSceneNameIdx != -1) {
				std::string sceneName = this->sceneManager->getScenes()[this->changingSceneNameIdx].first;
				this->sceneManager->setSceneName(this->changingSceneNameIdx, sceneName.substr(0, sceneName.size() - 1));
			}
			// Talker
			else if (this->currentDialogue != nullptr && this->clickedOnTalkerBox) {
				this->currentDialogue->talking = this->currentDialogue->talking.substr(0, this->currentDialogue->talking.size() - 1);
			}
			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
                // Move the line back
                if (this->currentMessageIdx > 0) {
                    std::string msg = this->currentDialogue->message;

                    if (msg.length() > 0 && this->currentMessageIdx > 0) {
                        std::string start = msg.substr(0, this->currentMessageIdx - 1);
                        std::string end = msg.substr(this->currentMessageIdx, msg.length());

                        this->currentMessageIdx--;
                        this->currentDialogue->message = start + end;
                    }
                }
			}
			// First answer box
			else if (this->currentDialogue != nullptr && this->clickedOnFirstAnswerBox) {
                std::string msg = this->currentDialogue->option1Text;

                if (msg.length() > 0 && this->currentMessageIdx > 0) {
                    std::string start = msg.substr(0, this->currentMessageIdx - 1);
                    std::string end = msg.substr(this->currentMessageIdx, msg.length());

                    this->currentDialogue->option1Text = start + end;
                    this->currentMessageIdx--;
                }
			}
			// Second answer box
			else if (this->currentDialogue != nullptr && this->clickedOnSecondAnswerBox) {
                std::string msg = this->currentDialogue->option2Text;

                if (msg.length() > 0 && this->currentMessageIdx > 0) {
                    std::string start = msg.substr(0, this->currentMessageIdx - 1);
                    std::string end = msg.substr(this->currentMessageIdx, msg.length());

                    this->currentDialogue->option2Text = start + end;
                    this->currentMessageIdx--;
                }
			}
			// Third answer box
			else if (this->currentDialogue != nullptr && this->clickedOnThirdAnswerBox) {
                std::string msg = this->currentDialogue->option3Text;

                if (msg.length() > 0 && this->currentMessageIdx > 0) {
                    std::string start = msg.substr(0, this->currentMessageIdx - 1);
                    std::string end = msg.substr(this->currentMessageIdx, msg.length());

                    this->currentDialogue->option3Text = start + end;
                    this->currentMessageIdx--;
                }
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
				if (this->currentSceneName.length() < 20) {
					if (this->currentSceneName.length() > 0) this->currentSceneName += " ";
				}

				this->duplicateSceneName = false;
                this->creationErrorMsg = "";
			}

			// Dialogue name
			else if (this->currentState == ProgramState::ADD_DIALOGUE) {
				if (this->currentDialogueName.length() < 20) {
					if (this->currentDialogueName.length() > 0) this->currentDialogueName += " ";
				}

				this->duplicateDialogueName = false;
                this->creationErrorMsg = "";
			}

			// Changing dialogue name
			else if (this->changingDialogueName) {
				if (this->currentDialogue->name.length() < 19) {
					if (this->currentDialogue->name.length() > 0) this->currentDialogue->name += " ";
				}
			}

			// Changing scene name
			else if (this->changingSceneNameIdx != -1) {
				std::string sceneName = this->sceneManager->getScenes()[this->changingSceneNameIdx].first;

				if (sceneName.length() < 19) {
					this->sceneManager->setSceneName(this->changingSceneNameIdx, sceneName + " ");
				}
			}

			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
                std::string msg = this->currentDialogue->message;
                std::string start = msg.substr(0, this->currentMessageIdx);
                std::string end = msg.substr(this->currentMessageIdx, msg.length());

                start += " ";

				this->currentDialogue->message = start + end;
                this->currentMessageIdx++;
			}

			// First answer box
			else if (this->currentDialogue != nullptr && this->clickedOnFirstAnswerBox) {
                std::string msg = this->currentDialogue->option1Text;
                std::string start = msg.substr(0, this->currentMessageIdx);
                std::string end = msg.substr(this->currentMessageIdx, msg.length());

                start += " ";

				this->currentDialogue->option1Text = start + end;
                this->currentMessageIdx++;
			}

			// Second answer box
			else if (this->currentDialogue != nullptr && this->clickedOnSecondAnswerBox) {
                std::string msg = this->currentDialogue->option2Text;
                std::string start = msg.substr(0, this->currentMessageIdx);
                std::string end = msg.substr(this->currentMessageIdx, msg.length());

                start += " ";

				this->currentDialogue->option2Text = start + end;
                this->currentMessageIdx++;
			}

			// Third answer box
			else if (this->currentDialogue != nullptr && this->clickedOnThirdAnswerBox) {
                std::string msg = this->currentDialogue->option3Text;
                std::string start = msg.substr(0, this->currentMessageIdx);
                std::string end = msg.substr(this->currentMessageIdx, msg.length());

                start += " ";

				this->currentDialogue->option3Text = start + end;
                this->currentMessageIdx++;
			}
		}

		// Paste
		if (this->clickedOnDialogueBox || this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox) {
			if (this->inputManager.isKeyDown(SDLK_LCTRL)) {
				if (this->inputManager.isKeyPressed(SDLK_v)) {
					if (!OpenClipboard(nullptr)) {
						std::cout << "Couldn't open clipboard!\n";
					}

					HANDLE h = GetClipboardData(CF_TEXT);
					if (h == nullptr) {
						std::cout << "Didn't get data!\n";
					}

					char *pszText = static_cast<char*>(GlobalLock(h));
					if (pszText == nullptr) {
						std::cout << "Couldn't cast to char!\n";
					}

					std::string text(pszText);

					CloseClipboard();

                    if (this->clickedOnDialogueBox) {
                        std::string msg = this->currentDialogue->message;
                        std::string start = msg.substr(0, this->currentMessageIdx);
                        std::string end = msg.substr(this->currentMessageIdx, msg.length());

                        start += text;
                        this->currentMessageIdx += text.length() - 1;
                        this->currentDialogue->message = start + end;
                    }
                    else if (this->clickedOnFirstAnswerBox) {
                        std::string msg = this->currentDialogue->option1Text;
                        std::string start = msg.substr(0, this->currentMessageIdx);
                        std::string end = msg.substr(this->currentMessageIdx, msg.length());

                        start += text;

                        this->currentMessageIdx += text.length() - 1;
                        this->currentDialogue->option1Text = start + end;
                    }
                    else if (this->clickedOnSecondAnswerBox) {
                        std::string msg = this->currentDialogue->option2Text;
                        std::string start = msg.substr(0, this->currentMessageIdx);
                        std::string end = msg.substr(this->currentMessageIdx, msg.length());

                        start += text;

                        this->currentMessageIdx += text.length() - 1;
                        this->currentDialogue->option2Text = start + end;
                    }
                    else if (this->clickedOnThirdAnswerBox) {
                        std::string msg = this->currentDialogue->option3Text;
                        std::string start = msg.substr(0, this->currentMessageIdx);
                        std::string end = msg.substr(this->currentMessageIdx, msg.length());

                        start += text;

                        this->currentMessageIdx += text.length() - 1;
                        this->currentDialogue->option3Text = start + end;
                    }
				}
			}
		}

		// Remove scene
		if (keyName == "Delete" && this->changingSceneNameIdx != -1) {
			this->deletingScene = true;
		}

		// Validate the text
		if (keyName == "Return") {
			// Changing dialogue name
			if (this->changingDialogueName) {
				this->changingDialogueName = false;
			}
			// Changing scene name
			else if (this->changingSceneNameIdx != -1) {
				this->changingSceneNameIdx = -1;
				this->deletingScene = false;
			}
			// Talker
			else if (this->currentDialogue != nullptr && this->clickedOnTalkerBox) {
				this->clickedOnTalkerBox = false;
			}
			// Message
			else if (this->currentDialogue != nullptr && this->clickedOnDialogueBox) {
				this->clickedOnDialogueBox = false;
                this->currentMessageIdx = 0;
			}
			// First answer box
			else if (this->currentDialogue != nullptr && this->clickedOnFirstAnswerBox) {
				this->clickedOnFirstAnswerBox = false;
				this->settingNextDialogue = false;
				this->selectedNextSceneIdx = -1;
				this->selectedAnswerBox = -1;
			}
			// Second answer box
			else if (this->currentDialogue != nullptr && this->clickedOnSecondAnswerBox) {
				this->clickedOnSecondAnswerBox = false;
				this->settingNextDialogue = false;
				this->selectedNextSceneIdx = -1;
				this->selectedAnswerBox = -1;
			}
			// Third answer box
			else if (this->currentDialogue != nullptr && this->clickedOnThirdAnswerBox) {
				this->clickedOnThirdAnswerBox = false;
				this->settingNextDialogue = false;
				this->selectedNextSceneIdx = -1;
				this->selectedAnswerBox = -1;
			}
			// Setting next dialogue
			else if (this->settingNextDialogue) {
				if (this->selectedDialogueIdx != -1 || this->selectedNextSceneIdx != -1) {
					std::string next = "";

					if (this->selectedNextSceneIdx != -1) {
						next = std::to_string(this->selectedNextSceneIdx) += "otherscene";
					}
					else {
						next = std::to_string(this->selectedDialogueIdx);
					}

					if (this->customNextDlg) {
						this->changingSettings = true;
						this->currentDialogue = nullptr;
						this->changingDialogueName = false;

						// Reset the scene & dialogue
						this->selectedSceneIdx = this->lastSceneIdx;
						this->selectedDialogueIdx = this->lastDialogueIdx;

						this->currentSceneListIdx = this->lastSceneListIdx;
						this->currentDialogueListIdx = this->lastDialogueListIdx;
					}
					else this->resetCurrentDialogue();
					
					if (this->selectedAnswerBox == 1) this->currentDialogue->option1Next = next;
					else if (this->selectedAnswerBox == 2) this->currentDialogue->option2Next = next;
					else if (this->selectedAnswerBox == 3) this->currentDialogue->option3Next = next;
					else this->lastDialogue->nextDialogue = next;

					this->customNextDlg = false;

					this->selectedAnswerBox = -1;
					this->settingNextDialogue = false;
					this->selectedNextSceneIdx = -1;
				}
			}
		}

		// Add the character to the text
		if (keyName != "Return" && keyName != "Space" && keyName != "Backspace" && keyName != "Up" && keyName != "Down" && keyName != "Delete") {
			// Scene name
			if (this->currentState == ProgramState::ADDSCENE) {
				if (this->currentSceneName.length() < 20) {
					this->currentSceneName += static_cast<char>(keyID);

					this->duplicateSceneName = false;
                    this->creationErrorMsg = "";
				}
			}
			// Dialogue name
			if (this->currentState == ProgramState::ADD_DIALOGUE) {
				if (this->currentDialogueName.length() < 20) {
					this->currentDialogueName += static_cast<char>(keyID);

					this->duplicateDialogueName = false;
                    this->creationErrorMsg = "";
				}
			}
			// Changing dialogue name
			else if (this->changingDialogueName) {
				if (this->currentDialogue->name.length() < 20) {
					this->currentDialogue->name += static_cast<char>(keyID);
				}
			}
			// Changing scene name
			else if (this->changingSceneNameIdx != -1) {
				std::string sceneName = this->sceneManager->getScenes()[this->changingSceneNameIdx].first;
				if (sceneName.length() < 20) {
					this->sceneManager->setSceneName(this->changingSceneNameIdx, sceneName + static_cast<char>(keyID));
				}
			}
			// Talker
			else if (this->clickedOnTalkerBox && keyName != "+" && keyName != "1" && keyName != "2" && keyName != "3"
				&& keyName != "4" && keyName != "5" && keyName != "6" && keyName != "7" && keyName != "8" && keyName != "9" && keyName != ","
				) {
				if (this->currentDialogue != nullptr && this->currentDialogue->talking.length() < 11) {
					this->currentDialogue->talking += static_cast<char>(keyID);
				}
			}
			// Message
			else if (this->currentDialogue != nullptr && (this->clickedOnDialogueBox || this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox || this->clickedOnTalkerBox)) {
                std::string msg = this->currentDialogue->message, start, end;

                if (this->clickedOnDialogueBox) {
                    start = msg.substr(0, this->currentMessageIdx);
                    end = msg.substr(this->currentMessageIdx, msg.length());
                }

                std::string option1 = this->currentDialogue->option1Text, option1Start, option1End;

                if (this->clickedOnFirstAnswerBox) {
                    option1Start = option1.substr(0, this->currentMessageIdx);
                    option1End = option1.substr(this->currentMessageIdx, option1.length());
                }

                std::string option2 = this->currentDialogue->option2Text, option2Start, option2End;

                if (this->clickedOnSecondAnswerBox) {
                    option2Start = option2.substr(0, this->currentMessageIdx);
                    option2End = option2.substr(this->currentMessageIdx, option2.length());
                }

                std::string option3 = this->currentDialogue->option3Text, option3Start, option3End;

                if (this->clickedOnThirdAnswerBox) {
                    option3Start = option3.substr(0, this->currentMessageIdx);
                    option3End = option3.substr(this->currentMessageIdx, option3.length());
                }

                if (!this->clickedOnTalkerBox) this->currentMessageIdx++;

                // Question mark
				if (keyName == "+" && (this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) {
					if (this->clickedOnDialogueBox) start += "?";
					else if (this->clickedOnFirstAnswerBox) option1Start += "?";
					else if (this->clickedOnSecondAnswerBox) option2Start += "?";
					else if (this->clickedOnThirdAnswerBox) option3Start += "?";
					else if (this->clickedOnTalkerBox && this->currentDialogue->talking.length() < 11) this->currentDialogue->talking += "?";
				}
				// Exclamation mark
				else if (keyName == "1" && (this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) {
					if (this->clickedOnDialogueBox) start += "!";
                    else if (this->clickedOnFirstAnswerBox) option1Start += "!";
                    else if (this->clickedOnSecondAnswerBox) option2Start += "!";
                    else if (this->clickedOnThirdAnswerBox) option3Start += "!";
				}
				// Upper case letter (while holding shift)
				else {
					char letter = ((this->inputManager.isKeyDown(SDLK_LSHIFT) || this->inputManager.isKeyDown(SDLK_RSHIFT))) ? toupper(static_cast<char>(keyID)) : static_cast<char>(keyID);

					if (!(this->inputManager.isKeyDown(SDLK_LCTRL) && this->inputManager.isKeyPressed(SDLK_v))) {
						if (this->clickedOnDialogueBox) start += letter;
                        else if (this->clickedOnFirstAnswerBox) option1Start += letter;
                        else if (this->clickedOnSecondAnswerBox) option2Start += letter;
                        else if (this->clickedOnThirdAnswerBox) option3Start += letter;
					}
				}

                if (this->clickedOnDialogueBox) {
                    this->currentDialogue->message = start + end;
                }

                if (this->clickedOnFirstAnswerBox) {
                    this->currentDialogue->option1Text = option1Start + option1End;
                }

                if (this->clickedOnSecondAnswerBox) {
                    this->currentDialogue->option2Text = option2Start + option2End;
                }

                if (this->clickedOnThirdAnswerBox) {
                    this->currentDialogue->option3Text = option3Start + option3End;
                }
			}
			// Dialogue name
			else {
				if (this->currentDialogueName.length() < 20) {
					this->currentDialogueName += static_cast<char>(keyID);
				}
			}
		}

		// Move scene up or down
		if (this->changingSceneNameIdx != -1) {
			if (keyName == "Up" && this->changingSceneNameIdx > 0) {
				this->sceneManager->moveSceneUp(this->changingSceneNameIdx);

				std::vector<std::string> bgs = this->sceneManager->getSceneBackgrounds();

				// Move backgrounds
				if (bgs.size() > this->changingSceneNameIdx) {
					std::string tempBg = bgs[this->changingSceneNameIdx - 1];
					bgs[this->changingSceneNameIdx - 1] = bgs[this->changingSceneNameIdx];
					bgs[this->changingSceneNameIdx] = tempBg;
					this->sceneManager->setSceneBackgrounds(bgs);
				}

				this->changingSceneNameIdx--;
				if (this->changingSceneNameIdx < this->currentSceneListIdx) this->currentSceneListIdx--;
			}
			else if (keyName == "Down" && this->changingSceneNameIdx < this->sceneManager->getScenes().size() - 1) {
				this->sceneManager->moveSceneDown(this->changingSceneNameIdx);

				std::vector<std::string> bgs = this->sceneManager->getSceneBackgrounds();

				// Move backgrounds
				if (bgs.size() > this->changingSceneNameIdx + 1) {
					std::string tempBg = bgs[this->changingSceneNameIdx + 1];
					bgs[this->changingSceneNameIdx + 1] = bgs[this->changingSceneNameIdx];
					bgs[this->changingSceneNameIdx] = tempBg;
					this->sceneManager->setSceneBackgrounds(bgs);
				}

				this->changingSceneNameIdx++;
				if (this->changingSceneNameIdx > this->currentSceneListIdx + 4) this->currentSceneListIdx++;
			}
		}

		// Move dialogues up or down
		if (this->currentDialogue != nullptr && this->selectedDialogueIdx != -1 && this->selectedSceneIdx != -1 &&
            !this->clickedOnFirstAnswerBox && !this->clickedOnSecondAnswerBox && !this->clickedOnThirdAnswerBox) {
			if (keyName == "Up" && this->selectedDialogueIdx > 0) {
				this->sceneManager->moveDialogueUp(this->selectedSceneIdx, this->selectedDialogueIdx);
				this->selectedDialogueIdx--;
				if (this->selectedDialogueIdx < this->currentDialogueListIdx) this->currentDialogueListIdx--;
			}
			else if (keyName == "Down" && this->selectedDialogueIdx < this->sceneManager->getDialogues(this->selectedSceneIdx).size() - 1) {
				this->sceneManager->moveDialogueDown(this->selectedSceneIdx, this->selectedDialogueIdx);
				this->selectedDialogueIdx++;
				if (this->selectedDialogueIdx > this->currentDialogueListIdx + 4) this->currentDialogueListIdx++;
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

			// Changing dialogue name
			if (this->changingDialogueName) {
				if (this->currentDialogue->name.length() > 0) {
					for (unsigned i = 0; i < this->currentDialogue->name.length(); i++) {
						if (i == 0) {
							this->currentDialogue->name[0] = toupper(this->currentDialogue->name[i]);
						}

						if (this->currentDialogue->name[i] == ' ') {
							if (i != this->currentDialogue->name.length() - 1) {
								this->currentDialogue->name[i + 1] = toupper(this->currentDialogue->name[i + 1]);
							}
						}
					}
				}
			}

			// Changing scene name
			if (this->changingSceneNameIdx != -1) {
				std::string sceneName = this->sceneManager->getScenes()[this->changingSceneNameIdx].first;

				if (sceneName.length() > 0) {
					for (unsigned i = 0; i < sceneName.length(); i++) {
						if (i == 0) {
							sceneName[0] = toupper(sceneName[i]);
						}

						if (sceneName[i] == ' ') {
							if (i != sceneName.length() - 1) {
								sceneName[i + 1] = toupper(sceneName[i + 1]);
							}
						}
					}

					this->sceneManager->setSceneName(this->changingSceneNameIdx, sceneName);
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
	else if (this->currentState == ProgramState::DELETE_DIALOGUE) this->drawDialogueDeletionScreen();

	this->spriteBatch.end();
	this->spriteBatch.renderBatch();

	if (this->currentState == ProgramState::FILESELECT) this->drawFileSelectTexts();
	else if (this->currentState == ProgramState::ADDSCENE || this->currentState == ProgramState::ADD_DIALOGUE) this->drawSceneCreationScreenTexts();
	else if (this->currentState == ProgramState::MAINSCREEN) this->drawMainScreenTexts();
	else if (this->currentState == ProgramState::DELETE_DIALOGUE) this->drawDialogueDeletionTexts();

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

	// Delete scene button
	if (this->deletingScene) {
		this->spriteBatch.draw(
			this->deleteSceneBtnDestRect,
			this->mainUvRect,
			Bengine::ResourceManager::getTexture("Textures/delete-button.png").id,
			0.0f,
			this->color
		);
	}

	// Cancel scene delete button
	if (this->deletingScene) {
		this->spriteBatch.draw(
			this->cancelSceneDeleteBtnDestRect,
			this->mainUvRect,
			Bengine::ResourceManager::getTexture("Textures/cancel-button-normal.png").id,
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
	if (!this->changingSettings && !(this->settingNextDialogue && this->selectedSceneIdx == -1) && !(this->settingNextDialogue && !this->customNextDlg)) {
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

		static const unsigned int greenBox = Bengine::ResourceManager::getTexture("Textures/scene-box-green.png").id;

		glm::vec4 originalDestRect = sceneBoxDestRect;
		for (unsigned i = 0; i < shownScenes.size(); i++) {
			if (this->shownSceneIndexes[i] == this->changingSceneNameIdx) {
				this->spriteBatch.draw(
					this->sceneBoxDestRect,
					this->mainUvRect,
					Bengine::ResourceManager::getTexture("Textures/scene-box-selected.png").id,
					0.0f,
					this->color
				);
			}
			else {
				this->spriteBatch.draw(
					this->sceneBoxDestRect,
					this->mainUvRect,
					(this->selectedNextSceneIdx == this->shownSceneIndexes[i]) ? greenBox : sceneBox,
					0.0f,
					this->color
				);
			}

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
			static const unsigned int blueBox = Bengine::ResourceManager::getTexture("Textures/scene-box-selected.png").id;

			for (unsigned i = 0; i < dialogues.size(); i++) {
				if (this->shownDialogueIndexes[i] == this->selectedDialogueIdx && this->settingNextDialogue) {
					this->spriteBatch.draw(
						destRect,
						this->mainUvRect,
						greenBox,
						0.0f,
						this->color
					);
				}
				else {
					this->spriteBatch.draw(
						destRect,
						this->mainUvRect,
						(this->shownDialogueIndexes[i] == this->selectedDialogueIdx) ? blueBox : sceneBox,
						0.0f,
						this->color
					);
				}
				
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
			(this->saved) ? Bengine::ResourceManager::getTexture("Textures/saved-button.png").id : saveBtn,
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

        std::string chara;
        if (lc.length() > 0) {
            chara = lc.substr(0, lc.find(","));
        }
        
        glm::vec4 destRect = this->leftCharDestRect;

        // Change size
        if (chara == "Fumiko-san") {
            destRect.z = 161;
        }
        else if (chara == "Hideo-kun" || chara == "Teemu-kun") {
            destRect.z = 201;
        }
        else if (chara == "Iwao-kun") {
            destRect.z = 177;
        }
        else if (chara == "Michiko-san") {
            destRect.z = 173;
        }

		// Left Character
		if (lc != "" && !this->currentDialogue->question) {
			this->spriteBatch.draw(
                destRect,
				this->flippedXUvRect,
				Bengine::ResourceManager::getTexture("../Visual Novel Game/Textures/Characters/" + lc.substr(0, lc.find(",")) + "/" + lc.substr(lc.find(",") + 2, lc.length())).id,
				0.0f,
				this->color
			);
		}

		std::string rc = this->currentDialogue->right;
        if (rc.length() > 0) {
            chara = rc.substr(0, rc.find(","));
        }

        destRect = this->rightCharDestRect;

        // Change size
        if (chara == "Fumiko-san") {
            destRect.z = 161;
        }
        else if (chara == "Hideo-kun" || chara == "Teemu-kun") {
            destRect.z = 201;
        }
        else if (chara == "Iwao-kun") {
            destRect.z = 177;
        }
        else if (chara == "Michiko-san") {
            destRect.z = 173;
        }

		// Right character
		if (this->currentDialogue->right != "" && !this->currentDialogue->question) {
			this->spriteBatch.draw(
                destRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("../Visual Novel Game/Textures/Characters/" + rc.substr(0, rc.find(",")) + "/" + rc.substr(rc.find(",") + 2, rc.length())).id,
				0.0f,
				this->color
			);
		}

        Bengine::GLTexture texture;
        /*if (this->clickedOnTalkerBox) texture = Bengine::ResourceManager::getTexture("Textures/TalkerBoxHighlight.png");*/
        /*else */texture = Bengine::ResourceManager::getTexture("Textures/TalkerBoxBlue.png");

		// Talker box
		if (this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->talker) {
			this->spriteBatch.draw(
				this->talkerBoxDestRect,
				this->mainUvRect,
				texture.id,
				0.0f,
				this->color
			);
		}

        if (this->clickedOnDialogueBox) texture = Bengine::ResourceManager::getTexture("Textures/TextBoxHighlight.png");
        else texture = Bengine::ResourceManager::getTexture("Textures/TextBoxBlueSolo.png");

		// Dialogue box
		if (this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx && this->currentDialogue->showTextBox) {
			this->spriteBatch.draw(
				this->textBoxDestRect,
				this->mainUvRect,
				texture.id,
				0.0f,
				this->color
			);
		}

        // The line
        if (this->clickedOnDialogueBox) {
            float xPos, yPos = this->textBoxDestRect.y + this->textBoxDestRect.w;
            yPos -= 39;

            std::vector<std::string> text = this->getWrappedText(this->currentDialogue->message, spriteFont, 550, 0.6f);
            
            int lengthSoFar = 0;
            for (size_t i = 0; i < text.size(); i++) {
                if (text[i].length() + lengthSoFar < this->currentMessageIdx) {
                    lengthSoFar += text[i].length();
                    yPos -= 26;
                }
                else {
                    int idx = this->currentMessageIdx - lengthSoFar;
                    std::string sub = text[i].substr(0, idx);
                    xPos = 23 + this->textBoxDestRect.x + this->spriteFont->measure(sub.c_str()).x * 0.6f;

                    break;
                }
            }

            this->spriteBatch.draw(
                glm::vec4(xPos, yPos, 1, 20),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                Bengine::ResourceManager::getTexture("Textures/line.png").id,
                0.0f,
                Bengine::ColorRGBA8(255, 255, 255, 255)
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

			// Determine third box color
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

			// Third answer box
			this->spriteBatch.draw(
				this->thirdAnswerBoxDestRect,
				this->mainUvRect,
				thirdBox,
				0.0f,
				this->color
			);

            float xPos = this->firstAnswerBoxDestRect.x + this->firstAnswerBoxDestRect.z / 2.0f, yPos;
            std::string m = "";
            glm::vec4 dim;

            if (this->currentDialogue->question) {
	            if (this->clickedOnFirstAnswerBox) {
	                m = this->currentDialogue->option1Text;
	                dim = this->firstAnswerBoxDestRect;
	            }
	            else if (this->clickedOnSecondAnswerBox) {
	                m = this->currentDialogue->option2Text;
	                dim = this->secondAnswerBoxDestRect;
	            }
	            else if (this->clickedOnThirdAnswerBox) {
	                m = this->currentDialogue->option3Text;
	                dim = this->thirdAnswerBoxDestRect;
	            }
	
	            float fontScale = 0.6f;
	
	            std::vector<std::string> text = this->getWrappedText(m, spriteFont, 520, fontScale);
	            
                // No text
                if (text.size() == 0 && (this->clickedOnFirstAnswerBox || this->clickedOnSecondAnswerBox || this->clickedOnThirdAnswerBox)) {
                    yPos = dim.y + dim.w / 2.0f - 6;

                    this->spriteBatch.draw(
                        glm::vec4(xPos, yPos, 1, 20),
                        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                        Bengine::ResourceManager::getTexture("Textures/line.png").id,
                        0.0f,
                        Bengine::ColorRGBA8(255, 255, 255, 255)
                    );
                }

                // One line of text
	            else if (text.size() == 1) {
                    // Move to the left
                    xPos = xPos + (this->spriteFont->measure(text[0].c_str()).x * fontScale / -2.0f);
                    std::string sub = text[0].substr(0, this->currentMessageIdx);
                    xPos += this->spriteFont->measure(sub.c_str()).x * fontScale - 1;

                    yPos = dim.y + dim.w / 2.0f - 6;
	
	                this->spriteBatch.draw(
	                    glm::vec4(xPos, yPos, 1, 20),
	                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	                    Bengine::ResourceManager::getTexture("Textures/line.png").id,
	                    0.0f,
	                    Bengine::ColorRGBA8(255, 255, 255, 255)
	                );
	            }

                // 2 Lines
                else {
                    yPos = dim.y + dim.w / 2.0f;

                    int lengthSoFar = 0;
                    for (size_t i = 0; i < text.size(); i++) {
                        if (text[i].length() + lengthSoFar < this->currentMessageIdx) {
                            lengthSoFar += text[i].length();
                            yPos -= 26;
                        }
                        else {
                            int idx = this->currentMessageIdx - lengthSoFar;
                            std::string sub = text[i].substr(0, idx);
                            xPos += (this->spriteFont->measure(text[i].c_str()).x * fontScale / -2.0f);
                            xPos += this->spriteFont->measure(sub.c_str()).x * 0.6f - 1;

                            break;
                        }
                    }

                    yPos += 5;

                    this->spriteBatch.draw(
                        glm::vec4(xPos, yPos, 1, 20),
                        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                        Bengine::ResourceManager::getTexture("Textures/line.png").id,
                        0.0f,
                        Bengine::ColorRGBA8(255, 255, 255, 255)
                    );
                }
            }
		}

		if (this->currentDialogue->question && this->sceneManager->getSceneBackgrounds().size() > this->selectedSceneIdx) {
			// Black boxes so can see next dialogue text
			glm::vec4 dim = this->firstAnswerBoxDestRect;

			this->spriteBatch.draw(
				glm::vec4(dim.x, dim.y + 85, 350, 35),
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/OptionBlackBox.png").id,
				0.0f,
				this->color
			);

			dim = this->secondAnswerBoxDestRect;

			this->spriteBatch.draw(
				glm::vec4(dim.x, dim.y + 85, 350, 35),
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/OptionBlackBox.png").id,
				0.0f,
				this->color
			);

			dim = this->thirdAnswerBoxDestRect;

			this->spriteBatch.draw(
				glm::vec4(dim.x, dim.y + 85, 350, 35),
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/OptionBlackBox.png").id,
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

        // Fade In checkbox
        if (this->lastDialogue->fadeIn)
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
        else
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

        this->spriteBatch.draw(
            this->fadeInCheckBox,
            this->mainUvRect,
            texture,
            0.0f,
            this->color
        );

        // Fade Out checkbox
        if (this->lastDialogue->fadeOut)
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
        else
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

        this->spriteBatch.draw(
            this->fadeOutCheckBox,
            this->mainUvRect,
            texture,
            0.0f,
            this->color
        );

		// Question checkbox
		if (this->lastDialogue->name.find("Question") != std::string::npos) {
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

		}

		// Custom next dialogue box
		if (this->lastDialogue->nextDialogue != "") {
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
		}
		else
			texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

		glm::vec4 movedDestRect = this->customNextCheckBox;
		movedDestRect.y += 80;

		this->spriteBatch.draw(
			(this->lastDialogue->name.find("Question") == std::string::npos) ? movedDestRect : this->customNextCheckBox,
			this->mainUvRect,
			texture,
			0.0f,
			this->color
		);

		movedDestRect = this->setNextCustomDlgDestRect;
		movedDestRect.y += 80;

		// Set next dialogue button
		if (this->lastDialogue->nextDialogue != "") {
			this->spriteBatch.draw(
				(this->lastDialogue->name.find("Question") == std::string::npos) ? movedDestRect : this->setNextCustomDlgDestRect,
				this->mainUvRect,
				Bengine::ResourceManager::getTexture("Textures/setnext.png").id,
				0.0f,
				this->color
			);
		}

        movedDestRect = this->startMusicCheckBox;
        if (this->lastDialogue->name.find("Question") == std::string::npos)
            movedDestRect.y += 80;

        if (this->lastDialogue->startMusic != "")
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
        else
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

        this->spriteBatch.draw(
            movedDestRect,
            this->mainUvRect,
            texture,
            0.0f,
            this->color
        );

        // Open music file button
        if (this->lastDialogue->startMusic != "") {
            glm::vec4 destRect = this->openMusicFileBtn;
            if (this->lastDialogue->name.find("Question") == std::string::npos)
                destRect.y += 80;

            this->spriteBatch.draw(
                destRect,
                this->mainUvRect,
                Bengine::ResourceManager::getTexture("Textures/openfile.png").id,
                0.0f,
                this->color
            );
        }

        // Open sound effect file button
        if (this->lastDialogue->soundEffect != "") {
            glm::vec4 destRect = this->openSoundEffectFileBtn;
            if (this->lastDialogue->name.find("Question") == std::string::npos)
                destRect.y += 80;

            this->spriteBatch.draw(
                destRect,
                this->mainUvRect,
                Bengine::ResourceManager::getTexture("Textures/openfile.png").id,
                0.0f,
                this->color
            );
        }

        movedDestRect = this->endMusicCheckBox;
        if (this->lastDialogue->name.find("Question") == std::string::npos)
            movedDestRect.y += 80;

        if (this->lastDialogue->endMusic)
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
        else
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

        this->spriteBatch.draw(
            movedDestRect,
            this->mainUvRect,
            texture,
            0.0f,
            this->color
        );

        movedDestRect = this->soundEffectCheckBox;
        if (this->lastDialogue->name.find("Question") == std::string::npos)
            movedDestRect.y += 80;

        if (this->lastDialogue->soundEffect != "")
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_checked.png").id;
        else
            texture = Bengine::ResourceManager::getTexture("Textures/checkbox_unchecked.png").id;

        this->spriteBatch.draw(
            movedDestRect,
            this->mainUvRect,
            texture,
            0.0f,
            this->color
        );

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


void MainProgram::drawDialogueDeletionScreen()
{
	// BG
	this->spriteBatch.draw(
		this->mainBgDestRect,
		this->mainUvRect,
		Bengine::ResourceManager::getTexture("Textures/dialoguedelete.png").id,
		0.0f,
		this->color
	);

	// Delete dialogue
	this->spriteBatch.draw(
		this->confirmDlgDeleteBtnDestRect,
		this->mainUvRect,
		Bengine::ResourceManager::getTexture("Textures/delete-button.png").id,
		0.0f,
		this->color
	);

	// Cancel dialogue delete
	this->spriteBatch.draw(
		this->cancelDlgDeleteBtnDestRect,
		this->mainUvRect,
		Bengine::ResourceManager::getTexture("Textures/cancel-button-normal.png").id,
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

	int currentY = 198;

	// Scene names, only show if a scene isn't opened
	if (this->selectedSceneIdx == -1) {
        // Scene list text
        sprintf_s(buffer, "%s", "Scene List");

        this->spriteFont->draw(
            this->fontBatch,
            buffer,
            glm::vec2(8, 615),
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

		for (unsigned i = 0; i < scenes.size(); i++) {
			std::string text = this->getBoxSizeText(scenes[i].first, 0.7f);

			sprintf_s(buffer, "%s", text.c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(25, this->screenHeight - currentY),
				glm::vec2(0.7f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);

			sprintf_s(buffer, "%s dialogues", std::to_string(scenes[i].second.size()).c_str());

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(25, this->screenHeight - currentY - 27),
				glm::vec2(0.5f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);

			currentY += 90;
		}
	}
	// Dialogue names, show if a scene is selected
	else {
		if (!this->changingSettings) {
            // Dialogue list text
            sprintf_s(buffer, "%s", "Dialogues");

            this->spriteFont->draw(
                this->fontBatch,
                buffer,
                glm::vec2(8, 615),
                glm::vec2(0.7f),
                0.0f,
                Bengine::ColorRGBA8(0, 0, 0, 255)
            );

            if (this->selectedDialogueIdx != -1) {
                // Dialogue content text
                sprintf_s(buffer, "%s", "Dialogue Content");

                this->spriteFont->draw(
                    this->fontBatch,
                    buffer,
                    glm::vec2(208, 615),
                    glm::vec2(0.7f),
                    0.0f,
                    Bengine::ColorRGBA8(0, 0, 0, 255)
                );
            }

			std::vector<Dialogue *> dialogues = this->getShownDialogues(this->sceneManager->getDialogues(this->selectedSceneIdx));

			for (unsigned i = 0; i < dialogues.size(); i++) {
				
				std::string text = this->getBoxSizeText(dialogues[i]->name, 0.7f);

				sprintf_s(buffer, "%s", text.c_str());

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

	// Write to change the scene name text
	if (this->changingSceneNameIdx != -1) {
		sprintf_s(buffer, "%s", "Write to change the name of the selected scene.");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(210, 540),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);
	}

	// Press delete to remove scene
	if (this->changingSceneNameIdx != -1) {
		sprintf_s(buffer, "%s", "Press delete to remove the selected scene.");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(210, 510),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);
	}

	// Are you sure delete scene
	if (this->deletingScene) {
		sprintf_s(buffer, "%s", "Are you sure you want to");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(210, 430),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		sprintf_s(buffer, "%s", "want to delete the selected scene ?");

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(210, 400),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);
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
		if (!(this->settingNextDialogue && !this->customNextDlg)) {
			sprintf_s(buffer, "%s", "Browse and click on a scene/dialogue on the left.");
		}
		else {
			sprintf_s(buffer, "%s", "Browse and click on a dialogue on the left.");
		}

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(220, 540),
			glm::vec2(0.7f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		if (!(this->settingNextDialogue && !this->customNextDlg)) {
			sprintf_s(buffer, "%s", "Confirm selected scene/dialogue with enter.");
		}
		else {
			sprintf_s(buffer, "%s", "Confirm selected dialogue with enter.");
		}

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
		else if (this->selectedNextSceneIdx != -1) {
			sprintf_s(buffer, "%s: %s", "Selected scene", this->sceneManager->getScenes()[this->selectedNextSceneIdx].first.c_str());
		}
		else {
			if (!(this->settingNextDialogue && !this->customNextDlg)) {
				sprintf_s(buffer, "%s", "No scene or dialogue selected.");
			}
			else {
				sprintf_s(buffer, "%s", "No dialogue selected.");
			}
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

        auto dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);

		// First option
        if (this->currentDialogue->option1Next != "Not Selected")
		    sprintf_s(buffer, "%s: %s", "Next Dialogue", dialogues[stoi(this->currentDialogue->option1Next)]->name.c_str());
        else
            sprintf_s(buffer, "%s", "Next Dialogue Not Selected");

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

		// Second option
        if (this->currentDialogue->option2Next != "Not Selected")
		    sprintf_s(buffer, "%s: %s", "Next Dialogue", dialogues[stoi(this->currentDialogue->option2Next)]->name.c_str());
        else
            sprintf_s(buffer, "%s", "Next Dialogue Not Selected");

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

		// Third option
        if (this->currentDialogue->option3Next != "Not Selected")
            sprintf_s(buffer, "%s: %s", "Next Dialogue", dialogues[stoi(this->currentDialogue->option3Next)]->name.c_str());
        else
            sprintf_s(buffer, "%s", "Next Dialogue Not Selected");

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
		sprintf_s(buffer, "%s", "Show dialogue box");

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
			glm::vec2(486, 550),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

        this->spriteFont->draw(
            this->fontBatch,
            "Fade In",
            glm::vec2(226, 470),
            glm::vec2(0.8f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

        this->spriteFont->draw(
            this->fontBatch,
            "Fade Out",
            glm::vec2(356, 470),
            glm::vec2(0.8f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

		// Question checkbox
		if (this->lastDialogue->name.find("Question") != std::string::npos) {
			sprintf_s(buffer, "%s", "Make this a question");

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(226, 390),
				glm::vec2(0.8f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);
		}

		// Custom next dialogue checkbox
		sprintf_s(buffer, "%s", "Set custom next dialogue");

		int y = 310;
		if (this->lastDialogue->name.find("Question") == std::string::npos) y += 80;

		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(226, y),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255)
		);

		// The custom next dialogue's name
		if (this->lastDialogue->nextDialogue != "") {
			std::string next = this->lastDialogue->nextDialogue;
			if (next != "Next Dialogue Not Set") {
				if (next.find("otherscene") == std::string::npos) {
					next = "Next Dialogue: " + this->sceneManager->getDialogues(this->selectedSceneIdx)[stoi(next)]->name;
				}
				else {
					std::string idx = next.substr(0, next.find("otherscene"));
					next = "Next Scene: " + this->sceneManager->getScenes()[stoi(idx)].first;
				}
			}

			sprintf_s(buffer, "%s", next.c_str());

			int y = 222;
			if (this->lastDialogue->name.find("Question") == std::string::npos) y += 80;

			this->spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(385, y),
				glm::vec2(0.8f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);
		}

        y = 157;
        if (this->lastDialogue->name.find("Question") == std::string::npos) y += 80;

        this->spriteFont->draw(
            this->fontBatch,
            "Begin Music",
            glm::vec2(226, y),
            glm::vec2(0.8f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

        this->spriteFont->draw(
            this->fontBatch,
            "End Music",
            glm::vec2(426, y),
            glm::vec2(0.8f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

        this->spriteFont->draw(
            this->fontBatch,
            "Sound Effect",
            glm::vec2(626, y),
            glm::vec2(0.8f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

        // Start music file name
        sprintf_s(buffer, "%s", this->lastDialogue->startMusic.c_str());

        glm::vec2 pos(this->openMusicFileBtn.x + this->screenWidth / 2, this->openMusicFileBtn.y + this->screenHeight / 2 - 35);
        if (this->lastDialogue->name.find("Question") == std::string::npos)
            pos.y += 80;

        this->spriteFont->draw(
            this->fontBatch,
            buffer,
            pos,
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(0, 0, 0, 255)
        );

        // Sound effect file name
        sprintf_s(buffer, "%s", this->lastDialogue->soundEffect.c_str());

        pos = glm::vec2(this->openSoundEffectFileBtn.x + this->screenWidth / 2, this->openSoundEffectFileBtn.y + this->screenHeight / 2 - 35);
        if (this->lastDialogue->name.find("Question") == std::string::npos)
            pos.y += 80;

        this->spriteFont->draw(
            this->fontBatch,
            buffer,
            pos,
            glm::vec2(0.7f),
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

	// If name is duplicate
	if (this->duplicateDialogueName) {
		sprintf_s(buffer, "%s", "A dialogue with that name already exists in this scene!");
	}
	else if (this->duplicateSceneName) {
		sprintf_s(buffer, "%s", "A scene with that name already exists!");
	}
    else if (this->creationErrorMsg != "") {
        sprintf_s(buffer, "%s", this->creationErrorMsg.c_str());
    }

	if (this->duplicateDialogueName || this->duplicateSceneName || this->creationErrorMsg != "") {
		this->spriteFont->draw(
			this->fontBatch,
			buffer,
			glm::vec2(this->screenWidth / 2, this->screenHeight / 2 + 150),
			glm::vec2(0.8f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255),
			Bengine::Justification::MIDDLE
		);
	}

	this->fontBatch.end();
	this->fontBatch.renderBatch();
}


void MainProgram::drawDialogueDeletionTexts()
{
	char buffer[256];

	GLint pLocation = this->shaderProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = this->fontCamera.getCameraMatrix();

	// Send the camera matrix
	glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

	this->fontBatch.begin();

	sprintf_s(buffer, "%s", "Are you sure you want to delete the dialogue:");

	this->spriteFont->draw(
		this->fontBatch,
		buffer,
		glm::vec2(this->screenWidth / 2, this->screenHeight / 2 + 50),
		glm::vec2(1.0f),
		0.0f,
		Bengine::ColorRGBA8(0, 0, 0, 255),
		Bengine::Justification::MIDDLE
	);

	sprintf_s(buffer, "%s%s%s", "'", this->lastDialogue->name.c_str(), "'?");

	this->spriteFont->draw(
		this->fontBatch,
		buffer,
		glm::vec2(this->screenWidth / 2, this->screenHeight / 2),
		glm::vec2(1.0f),
		0.0f,
		Bengine::ColorRGBA8(0, 0, 0, 255),
		Bengine::Justification::MIDDLE
	);

	this->fontBatch.end();
	this->fontBatch.renderBatch();
}


void MainProgram::onScroll(int yDir)
{
	if (yDir > 0) {
		if (this->selectedSceneIdx == -1) {
			if (this->currentSceneListIdx > 0) this->currentSceneListIdx--;
		}
		else if (this->selectedSceneIdx != -1 && !this->changingSettings) {
			if (this->currentDialogueListIdx > 0) this - currentDialogueListIdx--;
		}
	}
	else if (yDir < 0) {
		if (this->selectedSceneIdx == -1) {
			int a = this->currentSceneListIdx;
			int b = this->sceneManager->getScenesSize() - 5;

			if (a < b) this->currentSceneListIdx++;
		}
		else if (this->selectedSceneIdx != -1 && !this->changingSettings) {
			int a = this->currentDialogueListIdx;
			int b = this->sceneManager->getDialogues(this->selectedSceneIdx).size() - 5;

			if (a < b) this->currentDialogueListIdx++;
		}
	}
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


std::string MainProgram::getBoxSizeText(std::string text, float scale)
{
	int maxWidth = 165;

	int length = text.length() - 1;
	int idx = length;
	while (this->spriteFont->measure(text.c_str()).x * scale > maxWidth) {
		text = text.substr(0, idx);

		idx--;
	}

	if (idx < length) {
		do {
			text = text.substr(0, text.length() - 1);
		} while (text[text.length() - 1] == ' ');

		text += "...";
	}

	return text;
}


void MainProgram::resetEverything()
{
	this->sceneManager->saveToFile("../Visual Novel Game/Dialogues/" + this->currentFileName);
	this->currentState = ProgramState::FILESELECT;
	this->currentFileName = "";
	delete this->sceneManager;
	this->sceneManager = new SceneManager();

	this->shownSceneIndexes.clear();
	this->shownDialogueIndexes.clear();

	this->currentSceneName = "";
	this->currentDialogueName = "";
	this->currentFileName = "";

	this->clickedOnTalkerBox = false;
	this->clickedOnDialogueBox = false;
    this->currentMessageIdx = 0;

	this->clickedOnFirstAnswerBox = false;
	this->clickedOnSecondAnswerBox = false;
	this->clickedOnThirdAnswerBox = false;

	this->changingSettings = false;
	this->settingNextDialogue = false;
	this->customNextDlg = false;
	this->changingDialogueName = false;
}


void MainProgram::resetCurrentDialogue()
{
	this->currentDialogue = this->lastDialogue;
	this->selectedSceneIdx = this->lastSceneIdx;
	this->selectedDialogueIdx = this->lastDialogueIdx;

	this->currentSceneListIdx = this->lastSceneListIdx;
	this->currentDialogueListIdx = this->lastDialogueListIdx;
}


void MainProgram::submitDialogue()
{
    std::string left = "", right = "";

    // Set the characters to be the same as last dialogue's
    if (this->selectedSceneIdx != -1) {
        auto dialogues = this->sceneManager->getDialogues(this->selectedSceneIdx);
        
        if (dialogues.size() > 0) {
            left = dialogues[dialogues.size() - 1]->left;
            right = dialogues[dialogues.size() - 1]->right;
        }
    }

	// Create a new dialogue
	this->sceneManager->addDialogue(
		this->selectedSceneIdx,
		new Dialogue(
			this->sceneManager->getDialogues(this->selectedSceneIdx).size(),
			this->currentDialogueName,
			"",
			left,
			right,
			"",
			"",
			true,
			false,
			true,
            false,
            false,
            "",
            "",
            false,
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

    std::cout << "Current dialogue end shit: " << this->currentDialogue->endMusic << "\n";

	this->selectedDialogueIdx = dialogues.size() - 1;

	if (this->selectedDialogueIdx > 3) this->currentDialogueListIdx = this->selectedDialogueIdx - 4;
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
                i--;
				char currChar = text[i];

				// Go back to the last space so we don't crop in the middle of a word
				while (currChar != ' ') {
					i--;
					currChar = text[i];
				}
				crop = crop.substr(0, i + 1);
				lines.push_back(crop);
				text = text.substr(i + 1, text.length() - 1);
				break;
			}
		}
	}

	if (text != "") lines.push_back(text);

	return lines;
}


std::wstring MainProgram::getOpenFileName(std::string fileType)
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

    // Set the correct directory for the file type that is going to be opened
    std::string currentPath = currentDirectory;
    currentPath = currentPath.substr(0, currentPath.find("StoryEditor"));
    currentPath += "Visual Novel Game\\";

    if (fileType == "dlg") currentPath += "Dialogues";
    else if (fileType == "char") currentPath += "Textures\\Characters";
    else if (fileType == "music") currentPath += "Audio\\Music";
    else if (fileType == "soundeffect") currentPath += "Audio\\Sound Effects";
    else currentPath += "Textures\\Backgrounds";

    // Convert file path to a wide string
    std::wstring widePath = std::wstring(currentPath.begin(), currentPath.end());

	wchar_t buffer[MAX_PATH];

	OPENFILENAMEW ofn = { 0 };

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;

	// Show only the appropriate file type
	if (fileType == "char" || fileType == "bg") ofn.lpstrFilter = L"PNG Files\0*.png\0";
    else if (fileType == "music" || fileType == "soundeffect") ofn.lpstrFilter = L"MP3 Files\0*.mp3\0WAV Files\0*.wav\0OGG Files\0*.ogg\0";
	else ofn.lpstrFilter = L"Yaml Files\0*.yaml\0";

	ofn.nFilterIndex = 1;

    if (fileType == "char" || fileType == "bg")
        ofn.lpstrTitle = L"Select an image";
    else if (fileType == "music" || fileType == "soundeffect")
        ofn.lpstrTitle = L"Select an audio file";
    else
        ofn.lpstrTitle = L"Select a dialogue";

	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = widePath.c_str();
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