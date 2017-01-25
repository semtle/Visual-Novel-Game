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

	if (file.is_open()) {
		std::cout << "Saving...\n";

		if (this->scenes.size() > 0) {
			file << "Start:\n";
			file << "    next_scene: " << this->scenes[0].first << "\n";
		}

		for (unsigned i = 0; i < this->scenes.size(); i++) {
			file << this->scenes[i].first << ":\n";
			file << "    " << "Background: " << this->sceneBackgrounds[i] << "\n";

			std::vector<Dialogue *> dialogues = this->scenes[i].second;
			
			if (dialogues.size() > 0) {
				file << "    " << "Start:\n";
				file << "        " << "next: " << dialogues[0]->name << "\n";
			}

			for (unsigned j = 0; j < dialogues.size(); j++) {
				file << "    " << dialogues[j]->name << ":\n";

				// A normal dialogue
				if (!dialogues[j]->question) {
					if (dialogues[j]->talker)
						file << "        " << "talking: " << dialogues[j]->talking << "\n";

					if (dialogues[j]->left != "")
						file << "        " << "left: " << dialogues[j]->left << "\n";

					if (dialogues[j]->right != "")
						file << "        " << "right: " << dialogues[j]->right << "\n";

					if (dialogues[j]->showTextBox)
						file << "        " << "message: " << dialogues[j]->message << "\n";

					// Default next
					if (dialogues[j]->nextDialogue == "") {
						if (j != dialogues.size() - 1)
							file << "        " << "next: " << dialogues[j + 1]->name << "\n";
						else if (i != this->scenes.size() - 1)
							file << "        " << "next_scene: " << this->scenes[i + 1].first << "\n";
					}
					// Custom next
					else {
						std::string endPart = dialogues[j]->nextDialogue;

						if (endPart != "Next Dialogue Not Set") {
							endPart = endPart.substr(endPart.find(",") + 1, endPart.length());
							std::string sceneName = endPart.substr(0, endPart.find(","));

							// If the dialogue is in the current scene
							if (sceneName == this->scenes[i].first) {
								 std::string dialogueName = endPart.substr(endPart.find(",") + 1, endPart.length());
								 file << "        " << "next: " << dialogueName << "\n";
							}
							// If the dialogue is in another scene
							else {
								file << "        " << "next_scene: " << sceneName << "\n";
							}
						}

						
					}
				}
				// A question
				else {
					file << "        " << "asking: " << dialogues[j]->charAskingQuestion << "\n";

					// First option
					file << "        " << "Option 1:\n";
					file << "            " << "text: " << dialogues[j]->option1Text << "\n";
					file << "            " << "influence: " << dialogues[j]->option1Influence << "\n";
					file << "            " << "next: " << dialogues[j]->option1Next << "\n";

					// Second option
					file << "        " << "Option 2:\n";
					file << "            " << "text: " << dialogues[j]->option2Text << "\n";
					file << "            " << "influence: " << dialogues[j]->option2Influence << "\n";
					file << "            " << "next: " << dialogues[j]->option2Next << "\n";

					// Third option
					file << "        " << "Option 3:\n";
					file << "            " << "text: " << dialogues[j]->option3Text << "\n";
					file << "            " << "influence: " << dialogues[j]->option3Influence << "\n";
					file << "            " << "next: " << dialogues[j]->option3Next << "\n";
				}
			}
		}

		std::cout << "Story saved succesfully.\n";
	}
	else {
		std::cout << "File " << filePath << " was not found!\n";
		int x;
		std::cin >> x;
	}

	file.close();
}
