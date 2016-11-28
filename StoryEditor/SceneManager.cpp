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
		std::pair< int, std::pair< std::string, std::vector< Dialogue * > >>
		(
			this->scenes.size(), // Set the index of the scene to be 1 more than the previous
			std::pair< std::string, std::vector< Dialogue * >>
			(
				sceneName, // The name of the scene
				std::vector<Dialogue *>() // Empty vector of dialogues
			)
		)
	);
}


void SceneManager::addDialogue(int idx, Dialogue* dialogue)
{
	this->scenes[idx].second.push_back(dialogue);
}


void SceneManager::saveScenes(const std::string& filePath)
{
	std::ofstream file(filePath);

	for (unsigned i = 0; i < this->scenes.size(); i++) {
		file << this->scenes[i].first << "\n";
	}

	file.close();
}
