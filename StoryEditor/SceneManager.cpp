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
	// This is probably 100x more complicated than it should
	this->scenes.emplace(
		std::pair<int, std::pair<std::string, Dialogue>>
		(
			this->scenes.size(),
			std::pair<std::string, Dialogue>
			(
				sceneName,
				Dialogue(
					this->scenes.size(),
					"Talker",
					std::pair<std::string, std::string>
					("Left-Char", "Left-Image"),
					std::pair<std::string, std::string>
					("Right-Char", "Right-Image"),
					"Message",
					this->scenes.size() + 1
				)
			)
		)
	);
}


void SceneManager::saveScenes(const std::string& filePath)
{
	std::ofstream file(filePath);

	for (unsigned i = 0; i < this->scenes.size(); i++) {
		file << this->scenes[i].first << "\n";
	}

	file.close();
}
