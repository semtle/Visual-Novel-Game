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
#include <Bengine/InputManager.h>

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
		const std::string& playerName,
		const int& screenWidth,
		const int& screenHeight
	);

	void drawAnswerBoxes(
		Bengine::SpriteBatch& spriteBatch,
		const int& screenWidth,
		const int& screenHeight,
		int redBoxIdx = -1,
		int greenBoxIdx = -1
	);

	void drawAnswerTexts(
		Bengine::SpriteFont* spriteFont,
		Bengine::SpriteBatch* fontBatch,
		std::vector<std::string> answers,
		const int& screenWidth,
		const int& screenHeight
	);

	int processQuestionInputs(Bengine::InputManager* inputManager);
private:
	std::string message;
	std::string talker;
	const float MESSAGE_SCALE = 0.55f;
	std::vector<glm::vec2> answerBoxPositions;

    const int ANSWER_BOX_SPACE = 160;
	static const int ANSWER_BOX_WIDTH = 550;
	static const int ANSWER_BOX_HEIGHT = 86;

	std::vector<std::string> getWrappedText(std::string text, Bengine::SpriteFont* spriteFont, const float& maxLength, const float& fontScale);
};