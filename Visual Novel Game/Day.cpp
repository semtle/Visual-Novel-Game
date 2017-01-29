#include "Day.h"

Day::Day()
{
	
}


Day::~Day()
{
}

void Day::init(const std::string &fileName, Bengine::InputManager* manager, const std::map<std::string, Character *>& characters, const int& screenWidth, const int& screenHeight)
{
	this->fontBatch.init();

	this->characters = characters;

	std::string path = "Dialogues/" + fileName + ".yaml";
	this->file = YAML::LoadFile(path);

	this->sceneNames.push_back("0");
	std::cout << "Current scene: " << this->sceneNames[this->currentSceneIdx] << "\n";
	this->dialogueNames.push_back("0");

	// Get talker name
	std::string talker = "";
	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["talker"] != nullptr) {
		talker = this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["talker"].as<std::string>();
	}
	if (talker == "Player") {
		talker = this->playerName;
	}

	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["message"] != nullptr) {
		this->dialogues.push_back(new Dialogue(talker, this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["message"].as<std::string>()));
	}
	else {
		this->dialogues.push_back(new Dialogue(talker, ""));
	}
	
	this->inputManager = manager;

	this->camera.init(screenWidth, screenHeight);
	this->camera.setPosition(glm::vec2(screenWidth / 2, screenHeight / 2));
}


void Day::setDay(std::string dayName) {
	this->day = dayName;
}


void Day::setPlayerName(std::string playerName)
{
	this->playerName = playerName;
}


void Day::update()
{
	this->camera.update();
	if (this->wait) waitAfterQuestion();

	processInputs();
}


void Day::processInputs()
{
	/* When clicking the left mouse button, go to the next dialogue. */
	if (this->inputManager->isKeyDown(SDL_BUTTON_LEFT)) {
		std::cout << "Size: " << this->dialogues.size() << "\n";
		this->inputManager->releaseKey(SDL_BUTTON_LEFT);

		// Other than questions
		if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["name"].as<std::string>().find("Question") == std::string::npos) {
			// Go to next scene
			bool changeScene = false;
			if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["next"] == nullptr) {
				this->sceneNames.push_back(this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["next_scene"].as<std::string>());
				this->dialogueNames.push_back("0");
				changeScene = true;
			}
			// Go to next dialog
			else {
				this->dialogueNames.push_back(this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["next"].as<std::string>());
			}

			this->currentDialogueIdx++;

			if (changeScene) this->currentSceneIdx++;

			nextDialogue();
		}
		// Handle the question boxes
		else {
			// Get the index of the option that was clicked
			int idx = this->dialogues[this->currentDialogueIdx]->processQuestionInputs(this->inputManager);
			if (idx != -1) {
				// Reverse the index
				idx = abs(idx - 2);
				// Put the current option into string to access it from the YAML file
				this->optionString = "Option " + std::to_string(idx + 1);
				// Get the current option
				YAML::Node currNode = this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]][this->optionString];
				
				int influence = currNode["influence"].as<int>() - 1;
				std::cout << influence << "\n";

				if (influence == -1) {
					this->redBoxIdx = idx;
				}
				else if (influence == 1) {
					this->greenBoxIdx = idx;
				}

				// Wait a bit after answering the question so the player sees how the answer influenced the character
				this->wait = true;

				// Get the name of the character who was asking the question
				if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["asking"] != nullptr) {
					std::string asking = this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["asking"].as<std::string>();

					// Add (or substract) the influence from the character
					this->characters[asking]->addInfluence(influence);
				}
			}
		}

		std::cout << "Current dialogue name: " << this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["name"].as<std::string>() << "\n";
	}
	if (this->inputManager->isKeyDown(SDL_BUTTON_RIGHT)) {
		this->inputManager->releaseKey(SDL_BUTTON_RIGHT);
		if (this->currentDialogueIdx > 0) this->currentDialogueIdx--;
	}
}


void Day::doFading()
{

}

bool Day::exitGame()
{
	return false;
}

std::string Day::changeScene()
{
	return this->nextScene;
}

void Day::drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight)
{
	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["name"].as<std::string>().find("Question") == std::string::npos) {
		this->dialogues[this->currentDialogueIdx]->drawTexts(spriteFont, shaderProgram, &this->fontBatch, screenWidth, screenHeight);
	}
	else {
		this->answers.clear();
		for (unsigned i = 1; i <= 3; i++) {
			// Get the answer options to pass them on to the dialogue
			this->answers.push_back(this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["Option " + std::to_string(i)]["text"].as<std::string>());
		}

		// Draw the different options inside the option boxes
		this->dialogues[this->currentDialogueIdx]->drawAnswerTexts(spriteFont, &this->fontBatch, this->answers, screenWidth, screenHeight);
	}
}

void Day::drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight)
{
	static const unsigned int dayImage = Bengine::ResourceManager::getTexture("Textures/Backgrounds/Weekdays/" + this->day + ".png").id;
	unsigned int background = Bengine::ResourceManager::getTexture("Textures/Backgrounds/" + this->file[this->sceneNames[this->currentSceneIdx]]["Background"].as<std::string>()).id;
	static const glm::vec4 destRect(-screenWidth / 2, -screenHeight / 2, screenWidth, screenHeight);
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	std::string charName = "";
	int leftChar = -1;
	int rightChar = -1;
	int leftCharWidth, rightCharWidth;

	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["left"] != nullptr) {
		std::string left = this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["left"].as<std::string>();

		for (unsigned i = 0; i < left.length(); i++) {
			if (left[i] == ',') {
				charName = left.substr(0, i);

				// Get the image from the Character class
				leftChar = this->characters[charName]->getImages().find(left.substr(i + 2, left.size() - 1))->second;
				break;
			}
		}
	}

	if (charName == "Teemu-kun") leftCharWidth = 275;
	else leftCharWidth = 225;

	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["right"] != nullptr) {
		std::string right = this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["right"].as<std::string>();

		for (unsigned i = 0; i < right.length(); i++) {
			if (right[i] == ',') {
				charName = right.substr(0, i);

				// Get the image from the Character class
				rightChar = this->characters[charName]->getImages().find(right.substr(i + 2, right.size() - 1))->second;
				break;
			}
		}
	}

	if (charName == "Teemu-kun") rightCharWidth = 275;
	else rightCharWidth = 225;

	spriteBatch.draw(
		destRect,
		uvRect,
		background,
		0.0f,
		color
	);

	// TODO: Try to find another way to do this... Right now without this, the dialog box stays behind the background for some stupid fucking reason
	spriteBatch.end();
	spriteBatch.renderBatch();

	spriteBatch.begin();

	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["name"].as<std::string>().find("Question") == std::string::npos) {
		this->dialogues[this->currentDialogueIdx]->drawImages(spriteBatch, hudCamera, screenWidth, screenHeight, leftCharWidth, rightCharWidth, leftChar, rightChar);
	}
	else {
		this->dialogues[this->currentDialogueIdx]->drawAnswerBoxes(spriteBatch, screenWidth, screenHeight, this->redBoxIdx, this->greenBoxIdx);
	}
}


void Day::waitAfterQuestion()
{
	Uint32 ticks = SDL_GetTicks();

	while (SDL_GetTicks() - ticks < 1750) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				std::cout << "Should quit lol\n";
				break;
			}
		}
	}

	this->greenBoxIdx = -1;
	this->redBoxIdx = -1;

	this->wait = false;
	this->dialogueNames.push_back(this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]][this->optionString]["next"].as<std::string>());
	this->currentDialogueIdx++;
	
	nextDialogue();
}


void Day::nextDialogue()
{
	// Need to check if the message exists here, can't just pass it in because it can be null
	if (this->currentDialogueIdx == this->dialogues.size()) {

		// Get talker name
		std::string talker = "";
		if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["talker"] != nullptr) {
			talker = this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["talker"].as<std::string>();
		}
		if (talker == "Player") {
			talker = this->playerName;
		}

		if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["message"] != nullptr) {
			this->dialogues.push_back(new Dialogue(talker, this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]]["message"].as<std::string>()));
		}
		else {
			this->dialogues.push_back(new Dialogue(talker, ""));
		}
	}

	//this->currentDialogueIdx++;

	// Dialog was not found
	if (this->file[this->sceneNames[this->currentSceneIdx]][this->dialogueNames[this->currentDialogueIdx]] == nullptr) {
		Bengine::fatalError("The dialogue '" + this->dialogueNames[this->currentDialogueIdx] + "' in the scene '" + this->sceneNames[this->currentSceneIdx] + "' could not be found.");
	}
}