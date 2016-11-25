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
	this->scenes.emplace(std::pair<int, std::string>(this->scenes.size(), sceneName));
}


void SceneManager::saveScenes(const std::string& filePath)
{
	std::ofstream file(filePath);

	for (unsigned i = 0; i < this->scenes.size(); i++) {
		file << this->scenes[i] << "\n";
	}

	file.close();
}
