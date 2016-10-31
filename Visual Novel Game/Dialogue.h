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

	void drawImages(Bengine::SpriteBatch& spriteBatch, Bengine::Camera2D* hudCamera, Bengine::GLSLProgram* shaderProgram, const int& screenWidth, const int& screenHeight);
	void drawTexts(Bengine::SpriteFont* spriteFont, Bengine::GLSLProgram* shaderProgram, Bengine::SpriteBatch* fontBatch, const int& screenWidth, const int& screenHeight);
private:
	std::string message;
	std::string talker;
	const float MESSAGE_SCALE = 0.55f;

	std::vector<std::string> getWrappedText(std::string text, Bengine::SpriteFont* spriteFont);
};