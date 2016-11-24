#include "SceneManager.h"


SceneManager::SceneManager()
{
}


SceneManager::~SceneManager()
{
}


void SceneManager::update()
{
	// Empty, might store scenes here later
}


void SceneManager::addScene(std::string sceneName)
{
	this->scenes.push_back(sceneName);
}
