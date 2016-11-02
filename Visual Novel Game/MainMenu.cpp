#include "MainMenu.h"

MainMenu::MainMenu()
{
}


MainMenu::~MainMenu()
{
}


void MainMenu::init(Bengine::InputManager* manager, const std::map<std::string, Character *>& characters, const int& screenWidth, const int& screenHeight)
{
	this->fontBatch.init();
	this->inputManager = manager;
	this->camera.init(screenWidth, screenHeight);
	this->camera.setPosition(glm::vec2(screenWidth / 2, screenHeight / 2));
	this->backgrounds["menu"] = Bengine::ResourceManager::getTexture("Textures/Backgrounds/mainscreen.png").id;
	this->backgrounds["newgame"] = Bengine::ResourceManager::getTexture("Textures/Backgrounds/entername.png").id;
}


void MainMenu::update()
{
	this->camera.update();
	if (this->currentState == MenuState::MENU) this->currentBackground = "menu";
	else if (this->currentState == MenuState::NEWGAME) this->currentBackground = "newgame";
}


void MainMenu::enterName(unsigned int keyID)
{
	if (this->currentState == MenuState::NEWGAME) {
		std::string keyName = SDL_GetKeyName(keyID);
		static std::string allowedKeys[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Space", "Backspace", "Return" };

		bool keyAllowed = false;

		for (std::string name : allowedKeys) {
			if (name == keyName) keyAllowed = true;
		}

		if (!keyAllowed) return;

		// Remove the last character from the player's name
		if (keyName == "Backspace") {
			this->playerName = this->playerName.substr(0, this->playerName.size() - 1);
		}
		// Add a space to the player's name
		else if (keyName == "Space" && this->playerName.length() < 16) {
			if (this->playerName.length() > 1) this->playerName += " ";
		}
		// Confirm the player's name
		else if (keyName == "Return") {
			if (this->playerName.length() >= 3 && this->playerName.length() <= 16) {
				std::cout << "Name was valid.\n";
			}
		}
		// Add the character to the player's name
		else {
			if (/*this->playerName.length() < 16*/true) {
				this->playerName += static_cast<char>(keyID);
			}
		}
		
		// Capitalize first letters of the name
		this->playerName[0] = toupper(this->playerName[0]);

		for (unsigned i = 0; i < this->playerName.length(); i++) {
			if (this->playerName[i] == ' ') {
				if (i != this->playerName.length() - 1) {
					this->playerName[i + 1] = toupper(this->playerName[i + 1]);
				}
			}
		}
	}
}


void MainMenu::drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight)
{
	static const int newGameButton = Bengine::ResourceManager::getTexture("Textures/Visuals/newgame.png").id;
	static const int backButton = Bengine::ResourceManager::getTexture("Textures/Visuals/backbutton.png").id;

	static const Bengine::ColorRGBA8 color = Bengine::ColorRGBA8(255, 255, 255, 255);

	static const glm::vec4 bgDestRect(-screenWidth / 2, -screenHeight / 2, screenWidth, screenHeight);
	static const glm::vec4 backBtnDestRect(-51, -screenHeight / 2 + 46, 102, 26);

	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	if (this->currentState == MenuState::MENU) {
		spriteBatch.draw(
			bgDestRect,
			uvRect,
			this->backgrounds.find(this->currentBackground)->second,
			0.0f,
			color
		);
		menuInputs();
	}
	else if (this->currentState == MenuState::NEWGAME) {
		// Background
		spriteBatch.draw(
			bgDestRect,
			uvRect,
			this->backgrounds.find(this->currentBackground)->second,
			0.0f,
			color
		);

		// Back button
		spriteBatch.draw(
			backBtnDestRect,
			uvRect,
			backButton,
			0.0f,
			color
		);

		// Draw the new game button if the name is valid
		if (this->playerName.length() >= 3) {
			static const glm::vec2 NEWGAME_DIMENSIONS(195, 38);

			static glm::vec4 newGameRect(-NEWGAME_DIMENSIONS.x / 2, -98, NEWGAME_DIMENSIONS);

			spriteBatch.draw(
				newGameRect,
				uvRect,
				newGameButton,
				0.0f,
				color
			);
		}

		newGameInputs();
	}
}


void MainMenu::drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight)
{
	if (this->currentState == MenuState::NEWGAME) {
		/* PLAYERS NAME */
		if (this->playerName.length() > 0) {
			char buffer[256];

			// Set the camera matrix
			GLuint pLocation = shaderProgram->getUniformLocation("P");
			glm::mat4 cameraMatrix = this->camera.getCameraMatrix();

			// Send the camera matrix
			glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

			sprintf_s(buffer, "%s", this->playerName.c_str());

			this->fontBatch.begin();

			spriteFont->draw(
				this->fontBatch,
				buffer,
				glm::vec2(screenWidth / 2, screenHeight / 2 + 16),
				glm::vec2(1.0f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255),
				Bengine::Justification::MIDDLE
			);

			this->fontBatch.end();
			this->fontBatch.renderBatch();
		}
	}
}


void MainMenu::menuInputs()
{
	if (this->inputManager->isKeyDown(SDL_BUTTON_LEFT)) {
		glm::vec2 mouseCoords = this->inputManager->getMouseCoords();

		// All buttons are at the same x position and have same width
		if (mouseCoords.x > 305 && mouseCoords.x < 478) {

			// New game
			if (mouseCoords.y > 410 && mouseCoords.y < 453) {
				this->currentState = MenuState::NEWGAME;
				this->inputManager->releaseKey(SDL_BUTTON_LEFT);
			}

			// Load game
			if (mouseCoords.y > 460 && mouseCoords.y < 500) {

				this->inputManager->releaseKey(SDL_BUTTON_LEFT);
			}

			// Options
			if (mouseCoords.y > 507 && mouseCoords.y < 546) {

				this->inputManager->releaseKey(SDL_BUTTON_LEFT);
			}

			// Exit
			if (mouseCoords.y > 553 && mouseCoords.y < 593) {
				this->currentState = MenuState::QUIT;
			}
		}
	}
}


void MainMenu::newGameInputs()
{
	if (this->inputManager->isKeyDown(SDL_BUTTON_LEFT)) {
		glm::vec2 mouseCoords = this->inputManager->getMouseCoords();

		// New game
		if (mouseCoords.x > 302 && mouseCoords.x < 497) {
			if (mouseCoords.y > 360 && mouseCoords.y < 398) {
				newGame();
				this->inputManager->releaseKey(SDL_BUTTON_LEFT);
			}
		}

		// Back
		if (mouseCoords.x > 349 && mouseCoords.x < 451) {
			if (mouseCoords.y > 528 && mouseCoords.y < 554) {
				//this->playerName = "";
				this->currentState = MenuState::MENU;
				this->inputManager->releaseKey(SDL_BUTTON_LEFT);
			}
		}
	}
}


void MainMenu::newGame()
{
	if (this->playerName.length() >= 3) {
		this->nextScene = "monday";
	}
}
