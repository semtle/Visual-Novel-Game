#include "Day.h"

Day::Day()
{
	
}


Day::~Day()
{
}

void Day::init(Bengine::InputManager* manager, const std::map<std::string, Character *>& characters, const int& screenWidth, const int& screenHeight)
{
	this->fontBatch.init();

	this->characters = characters;

	std::string path = "Dialogues/" + this->day + ".yaml";
	this->file = YAML::LoadFile(path);

	std::cout << "BEFORE";
	this->currentScene = this->file["Start"]["next_scene"].as<std::string>();
	std::cout << "Current scene: " << this->currentScene;
	this->currentDialogue = "Start";

	if (this->file[this->currentScene][this->currentDialogue]["message"] != nullptr) {
		this->dialogues.push_back(new Dialogue(this->playerName, this->file[this->currentScene][this->currentDialogue]["message"].as<std::string>()));
	}
	else {
		this->dialogues.push_back(new Dialogue(this->playerName, ""));
	}
	
	this->inputManager = manager;

	this->camera.init(screenWidth, screenHeight);
	this->camera.setPosition(glm::vec2(screenWidth / 2, screenHeight / 2));
	std::cout << std::endl;
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
	processInputs();
}


void Day::processInputs()
{
	/* When clicking the left mouse button, go to the next dialogue. */
	if (this->inputManager->isKeyDown(SDL_BUTTON_LEFT)) {
		this->inputManager->releaseKey(SDL_BUTTON_LEFT);

		// Other than questions
		if (this->currentDialogue != "Question") {
			// Go to next scene
			if (this->file[this->currentScene][this->currentDialogue]["next"] == nullptr) {
				this->currentScene = this->file[this->currentScene][this->currentDialogue]["next_scene"].as<std::string>();
				this->currentDialogue = "Start";
				std::cout << "Changed scene to " << this->currentScene << "\n";
				std::cout << "Change dialog to " << this->currentDialogue << "\n";
			}
			// Go to next dialog
			else {
				this->currentDialogue = this->file[this->currentScene][this->currentDialogue]["next"].as<std::string>();
			}

			// Need to check if the message exists here, can't just pass it in because it can be null
			if (this->file[this->currentScene][this->currentDialogue]["message"] != nullptr) {
				this->dialogues.push_back(new Dialogue(this->playerName, this->file[this->currentScene][this->currentDialogue]["message"].as<std::string>()));
			}
			else {
				this->dialogues.push_back(new Dialogue(this->playerName, ""));
			}

			// This probably shouldn't be done with a vector since we're only using one element
			// unless later we want to be able to go back to the previous dialogues (maybe will on questions at least)
			delete this->dialogues[0];
			this->dialogues[0] = this->dialogues.back();
			this->dialogues.pop_back();

			// Dialog was not found
			if (this->file[this->currentScene][this->currentDialogue] == nullptr) {
				Bengine::fatalError("The dialogue '" + this->currentDialogue + "' in the scene '" + this->currentScene + "' could not be found.");
			}
		}
		// Handle the question boxes
		else {
			// Get the index of the option that was clicked
			int idx = this->dialogues[0]->processQuestionInputs(this->inputManager);
			if (idx != -1) {
				std::cout << this->answers[abs(idx - 2)] << "\n";
			}
		}
	}
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
	if (this->currentDialogue != "Question") {
		this->dialogues[0]->drawTexts(spriteFont, shaderProgram, &this->fontBatch, screenWidth, screenHeight);
	}
	else {
		this->answers.clear();
		for (unsigned i = 1; i <= 3; i++) {
			// Get the answer options to pass them on to the dialogue
			this->answers.push_back(this->file[this->currentScene][this->currentDialogue]["Option " + std::to_string(i)]["text"].as<std::string>());
		}

		// Draw the different options inside the option boxes
		this->dialogues[0]->drawAnswerTexts(spriteFont, &this->fontBatch, this->answers, screenWidth, screenHeight);
	}
}

void Day::drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight)
{
	unsigned int background = Bengine::ResourceManager::getTexture("Textures/Backgrounds/" + this->file[this->currentScene]["Background"].as<std::string>() + ".png").id;
	static const glm::vec4 destRect(-screenWidth / 2, -screenHeight / 2, screenWidth, screenHeight);
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	std::string charName = "";
	int leftChar = -1;
	int rightChar = -1;
	int leftCharWidth, rightCharWidth;

	if (this->file[this->currentScene][this->currentDialogue]["left"] != nullptr) {
		std::string left = this->file[this->currentScene][this->currentDialogue]["left"].as<std::string>();

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

	if (this->file[this->currentScene][this->currentDialogue]["right"] != nullptr) {
		std::string right = this->file[this->currentScene][this->currentDialogue]["right"].as<std::string>();

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

	if (this->currentDialogue != "Question") {
		this->dialogues[0]->drawImages(spriteBatch, hudCamera, screenWidth, screenHeight, leftCharWidth, rightCharWidth, leftChar, rightChar);
	}
	else {
		this->dialogues[0]->drawAnswerBoxes(spriteBatch, screenWidth, screenHeight);
	}
}