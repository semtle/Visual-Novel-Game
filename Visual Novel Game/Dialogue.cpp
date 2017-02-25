#include "Dialogue.h"


Dialogue::Dialogue(std::string talker, std::string message)
{
	this->talker = talker;
	this->message = message;
}


Dialogue::~Dialogue()
{
}


void Dialogue::drawImages(
	Bengine::SpriteBatch& spriteBatch,
	Bengine::Camera2D* hudCamera,
	const int& screenWidth,
	const int& screenHeight,
	const int& leftCharWidth,
	const int& rightCharWidth,
	const int& leftChar,
	const int& rightChar)
{
	Bengine::GLTexture talkerBox = Bengine::ResourceManager::getTexture("Textures/Visuals/NameBlue.png");
	static const glm::vec4 talkerBoxDestRect(-screenWidth / 2 + 20, -screenHeight / 2 + 157, 159, 66);

	unsigned static const int messageBox = Bengine::ResourceManager::getTexture("Textures/Visuals/BlueboxSLQ.png").id;
	static const glm::vec4 messageBoxRect(-screenWidth / 2 + 6, -screenHeight / 2, 788, 177);

	glm::vec4 leftCharRect(-screenWidth  / 2 + 50, -screenHeight / 2, leftCharWidth, 600);
	glm::vec4 rightCharRect(screenWidth / 2 - 300, -screenHeight / 2, rightCharWidth, 600);

	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const glm::vec4 leftCharUvRect(0.0f, 0.0f, -1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	// The character on the left
	if (leftChar != -1) {
		spriteBatch.draw(
			leftCharRect,
			leftCharUvRect,
			leftChar,
			0.0f,
			color
		);
	}

	// The character on the right
	if (rightChar != -1) {
		spriteBatch.draw(
			rightCharRect,
			uvRect,
			rightChar,
			0.0f,
			color
		);
	}

	/* The talker box */
	if (this->talker != "") {
		spriteBatch.draw(
			talkerBoxDestRect,
			uvRect,
			talkerBox.id,
			0.0f,
			color
		);
	}

	/* The dialogue box */
	if (this->message != "") {
		spriteBatch.draw(
			messageBoxRect,
			uvRect,
			messageBox,
			0.0f,
			color
		);
	}
}


void Dialogue::drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, Bengine::SpriteBatch* fontBatch, const std::string& playerName, const int& screenWidth, const int& screenHeight)
{
	if (this->message != "") {
		static const glm::vec2 bottomLeft(-screenWidth / 2, -screenHeight / 2);

		fontBatch->begin();

		char buffer[256];

		sprintf_s(buffer, "%s", this->talker.c_str());

		/* Talkers name */
		spriteFont->draw(
			*fontBatch,
			buffer,
			bottomLeft + glm::vec2(98, 182),
			glm::vec2(0.6f),
			0.0f,
			Bengine::ColorRGBA8(0, 0, 0, 255),
			Bengine::Justification::MIDDLE
		);

		// Replace all "player's" with player's name
		std::string::size_type n = 0;
		while ((n = this->message.find("player", n)) != std::string::npos) {
			this->message.replace(n, 6, playerName);
			n += 6;
		}
		n = 0;
		while ((n = this->message.find("Player", n)) != std::string::npos) {
			this->message.replace(n, 6, playerName);
			n += 6;
		}

		// Get the lines from the message if it overflows the dialog box
		std::vector<std::string> wrappedMessage = getWrappedText(this->message, spriteFont, 700.0f, this->MESSAGE_SCALE);

		for (unsigned i = 0; i < wrappedMessage.size(); i++) {
			sprintf_s(buffer, "%s", wrappedMessage[i].c_str());

			/* The main text inside the dialogue box */
			spriteFont->draw(
				*fontBatch,
				buffer,
				bottomLeft + glm::vec2(40, 127 - (i * (spriteFont->getFontHeight() / 1.5f))),
				glm::vec2(0.55f),
				0.0f,
				Bengine::ColorRGBA8(0, 0, 0, 255)
			);
		}

		fontBatch->end();
		fontBatch->renderBatch();
	}
}


void Dialogue::drawAnswerBoxes(
	Bengine::SpriteBatch& spriteBatch,
	const int& screenWidth,
	const int& screenHeight,
	int redBoxIdx,
	int greenBoxIdx)
{
	// Load the boxes
	static const unsigned int blueBox = Bengine::ResourceManager::getTexture("Textures/Visuals/BlueOptionBoxSolo.png").id;
	static const unsigned int greenBox = Bengine::ResourceManager::getTexture("Textures/Visuals/GreenOptionBoxSolo.png").id;
	static const unsigned int redBox = Bengine::ResourceManager::getTexture("Textures/Visuals/RedOptionBoxSolo.png").id;

	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);


	int idx = 0;
	// Draw 3 blue boxes
	for (int y = this->ANSWER_BOX_SPACE; y >= -this->ANSWER_BOX_SPACE; y -= this->ANSWER_BOX_SPACE) {
		float x = -this->ANSWER_BOX_WIDTH / 2;

		// Red box - bad influence
		if (redBoxIdx == idx) {
			spriteBatch.draw(
				glm::vec4(x, y - this->ANSWER_BOX_HEIGHT / 2, this->ANSWER_BOX_WIDTH, this->ANSWER_BOX_HEIGHT),
				uvRect,
				redBox,
				0.0f,
				color
			);
		}
		// Green box - good influence
		else if (greenBoxIdx == idx) {
			spriteBatch.draw(
				glm::vec4(x, y - this->ANSWER_BOX_HEIGHT / 2, this->ANSWER_BOX_WIDTH, this->ANSWER_BOX_HEIGHT),
				uvRect,
				greenBox,
				0.0f,
				color
			);
		}
		// Blue box - neutral influence
		else {
			spriteBatch.draw(
				glm::vec4(x, y - this->ANSWER_BOX_HEIGHT / 2, this->ANSWER_BOX_WIDTH, this->ANSWER_BOX_HEIGHT),
				uvRect,
				blueBox,
				0.0f,
				color
			);
		}

		if (this->answerBoxPositions.size() < 3) {
			// Get the locations of the boxes and push them to the position vector
			float xp = (screenWidth + x * 2) / 2;
			float yp = screenHeight / 2 + y - this->ANSWER_BOX_HEIGHT / 2;
			this->answerBoxPositions.push_back(glm::vec2(xp, yp));
		}

		idx++;
	}
}


void Dialogue::drawAnswerTexts(Bengine::SpriteFont* spriteFont, Bengine::SpriteBatch* fontBatch, std::vector<std::string> answers, const int& screenWidth, const int& screenHeight)
{
	// The buffer that holds the texts
	char buffer[256];

	static const float FONT_SCALE = 0.6f;
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(0, 0, 0, 255);

	fontBatch->begin();

	// Draw the answers inside the boxes
	unsigned i = 0;
	for (int y = this->ANSWER_BOX_SPACE; y >= -this->ANSWER_BOX_SPACE; y -= this->ANSWER_BOX_SPACE) {
		std::vector<std::string> wrappedText = getWrappedText(answers[i], spriteFont, this->ANSWER_BOX_WIDTH - 50, FONT_SCALE);

		// If the answer has more than 1 lines
		if (wrappedText.size() > 1) {
			for (unsigned j = 0; j < wrappedText.size(); j++) {
				sprintf_s(buffer, "%s", wrappedText[j].c_str());

				spriteFont->draw(
					*fontBatch,
					buffer,
					glm::vec2(0.0f, y - j * ((spriteFont->getFontHeight() + 5) * FONT_SCALE)),
					glm::vec2(FONT_SCALE),
					0.0f,
					color,
					Bengine::Justification::MIDDLE
				);
			}
		}
		// If the answer only has 1 line
		else {
			sprintf_s(buffer, "%s", wrappedText[0].c_str());

			spriteFont->draw(
				*fontBatch,
				buffer,
				glm::vec2(0.0f, y - spriteFont->getFontHeight() * FONT_SCALE / 2),
				glm::vec2(FONT_SCALE),
				0.0f,
				color,
				Bengine::Justification::MIDDLE
			);
		}

		i++;
	}

	fontBatch->end();
	fontBatch->renderBatch();
}


int Dialogue::processQuestionInputs(Bengine::InputManager* inputManager)
{
	glm::vec2 mouseCoords = inputManager->getMouseCoords();

	std::cout << "Size: " << this->answerBoxPositions.size() << "\n";

	for (unsigned i = 0; i < this->answerBoxPositions.size(); i++) {
		if (mouseCoords.x > answerBoxPositions[i].x && mouseCoords.x < answerBoxPositions[i].x + this->ANSWER_BOX_WIDTH) {
			if (mouseCoords.y > answerBoxPositions[i].y && mouseCoords.y < answerBoxPositions[i].y + this->ANSWER_BOX_HEIGHT) {
				return i;
			}
		}
	}
	
	return -1;
}


std::vector<std::string> Dialogue::getWrappedText(std::string text, Bengine::SpriteFont* spriteFont, const float& maxLength, const float& fontScale)
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