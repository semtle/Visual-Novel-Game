#include "SceneManager.h"



SceneManager::SceneManager(Bengine::InputManager* InputManager)
{
	this->inputManager = InputManager;
}


SceneManager::~SceneManager()
{
}


void SceneManager::update()
{
	if (!this->isSceneWindowOpen) this->processInput();
	else {
		std::cout << "Im updating this useful stuff!!: ) \n";
		this->sceneCamera.update();
		this->inputManager->update();
		this->processSceneWindowInput();
	}
}


void SceneManager::processInput()
{
	glm::vec2 mouseCoords = this->inputManager->getMouseCoords();

	// Mouse is inside the 'add new' button
	if (mouseCoords.x > 50 && mouseCoords.x < 199) {
		if (mouseCoords.y > 700 && mouseCoords.y < 738) {
			if (this->inputManager->isKeyDown(SDL_BUTTON_LEFT)) {
				this->inputManager->releaseKey(SDL_BUTTON_LEFT);
				this->createSceneWindow();
				this->handleSceneWindow();
			}
		}
	}
}


void SceneManager::createSceneWindow()
{
	// Window
	this->sceneWindow.create("stupid fags lol", this->sceneWindowWidth, this->sceneWindowHeight, 0, false);

	// Shaders
	this->shaderProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	this->shaderProgram.addAttribute("vertexPosition");
	this->shaderProgram.addAttribute("vertexColor");
	this->shaderProgram.addAttribute("vertexUV");
	this->shaderProgram.linkShaders();

	// Camera
	this->sceneCamera.init(this->sceneWindowWidth, this->sceneWindowHeight);

	// Sprite batch
	this->spriteBatch.init();

	this->isSceneWindowOpen = true;
}


void SceneManager::handleSceneWindow()
{
	// While the window is open, handle it
	while (this->isSceneWindowOpen) {
		this->update();
		this->drawSceneWindow();
	}

	// Close the window
	this->sceneWindow.destroy();
}


void SceneManager::processSceneWindowInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// Here so that the window won't freeze :-)
	}

	glm::vec2 mouseCoords = this->inputManager->getMouseCoords();
}


void SceneManager::drawSceneWindow()
{
	// Set the base depth to 1.0
	glClearDepth(1.0f);

	// Clear the depth and color buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader program
	this->shaderProgram.use();

	// Set the first texture to be active
	glActiveTexture(GL_TEXTURE0);

	GLint textureLocation = this->shaderProgram.getUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	// Set the camera matrix
	GLint pLocation = this->shaderProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = this->sceneCamera.getCameraMatrix();

	// Send the camera matrix
	glUniformMatrix4fv(pLocation, 1, false, &cameraMatrix[0][0]);

	static const unsigned int main_background = Bengine::ResourceManager::getTexture("Textures/main-bg.png").id;
	static const unsigned int add_new_button = Bengine::ResourceManager::getTexture("Textures/addnew.png").id;
	static const glm::vec4 bgDestRect(-this->sceneWindowWidth / 2, -this->sceneWindowHeight / 2, this->sceneWindowWidth, this->sceneWindowHeight);
	static const glm::vec4 addNewDestRect(-this->sceneWindowWidth / 2 + 50, -this->sceneWindowHeight / 2 + 30, 149, 38);
	static const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	static const Bengine::ColorRGBA8 color(255, 255, 255, 255);

	this->spriteBatch.begin();
	this->spriteBatch.draw(
		bgDestRect,
		uvRect,
		main_background,
		0.0f,
		color
	);
	/*
	this->spriteBatch.draw(
		cancelBtnRect,
		uvRect,
		this->cancelButton,
		0.0f,
		color
	);*/

	this->spriteBatch.end();
	this->spriteBatch.renderBatch();

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the shader program
	this->shaderProgram.unuse();

	// Swap the buffer
	this->sceneWindow.swapBuffer();
}
