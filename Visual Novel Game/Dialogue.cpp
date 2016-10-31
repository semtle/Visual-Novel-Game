#include "Dialogue.h"


Dialogue::Dialogue(std::string talker, std::string message)
{
	this->talker = talker;
	this->message = message;
}


Dialogue::~Dialogue()
{
}


void Dialogue::drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight)
{
	unsigned static const int box = Bengine::ResourceManager::getTexture("Textures/Visuals/TextBoxBlue.png").id;
	static const glm::vec4 destRect(-screenWidth / 2, -screenHeight / 2, screenWidth, screenHeight);
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	/* The dialogue box */
	if (this->message != "") {
		spriteBatch.draw(
			destRect,
			uvRect,
			box,
			0.0f,
			color
		);
	}
}


void Dialogue::drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, Bengine::SpriteBatch* fontBatch, const int& screenWidth, const int& screenHeight)
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

		// Get the lines from the message if it overflows the dialog box
		std::vector<std::string> wrappedMessage = getWrappedText(this->message, spriteFont);

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


std::vector<std::string> Dialogue::getWrappedText(std::string text, Bengine::SpriteFont* spriteFont)
{
	const static float MAX_LENGTH = 700.0f;
	std::vector<std::string> lines;

	while (spriteFont->measure(text.c_str()).x * this->MESSAGE_SCALE > MAX_LENGTH) {
		// The current line that will be added
		std::string crop = "";
		for (unsigned i = 0; i < text.length(); i++) {
			crop += text[i];

			// If the line doesn't fit the box, it means that we should end the current line
			if (spriteFont->measure(crop.c_str()).x * this->MESSAGE_SCALE > MAX_LENGTH) {
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