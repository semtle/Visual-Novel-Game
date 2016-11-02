#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <Bengine/SpriteBatch.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/Camera2D.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <Bengine/SpriteFont.h>

class Dialogue
{
public:
	Dialogue(std::string talker, std::string message);
	~Dialogue();

	void drawImages(
		Bengine::SpriteBatch& spriteBatch,
		Bengine::Camera2D* hudCamera,
		const int& screenWidth,
		const int& screenHeight,
		const int& leftCharWidth,
		const int& rightCharWidth,
		const int& leftChar = -1,
		const int& rightChar = -1
	);

	void drawTexts(
		Bengine::SpriteFont* spriteFont,
		Bengine::GLSLProgram* shaderProgram,
		Bengine::SpriteBatch* fontBatch,
		const int& screenWidth,
		const int& screenHeight
	);

	void drawAnswerBoxes(
		Bengine::SpriteBatch& spriteBatch,
		const int& screenWidth,
		const int& screenHeight
	);

	void drawAnswerTexts(
		Bengine::SpriteFont* spriteFont,
		Bengine::SpriteBatch* fontBatch,
		std::vector<std::string> answers,
		const int& screenWidth,
		const int& screenHeight
	);

	std::vector<glm::vec4> getAnswerBoxDimensions() const {
		std::vector<glm::vec4> v;
		for (int y = this->ANSWER_BOX_SPACE; y >= -this->ANSWER_BOX_SPACE; y -= this->ANSWER_BOX_SPACE) {
			/* CONTINUE HERE, INPUT PROCESSING FOR OPTION BOXES */
		}
	}
private:
	std::string message;
	std::string talker;
	const float MESSAGE_SCALE = 0.55f;
	const int ANSWER_BOX_SPACE = 160;

	std::vector<std::string> getWrappedText(std::string text, Bengine::SpriteFont* spriteFont, const float& maxLength, const float& fontScale);
};