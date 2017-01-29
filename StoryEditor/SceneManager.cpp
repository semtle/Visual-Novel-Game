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


void SceneManager::moveSceneUp(int index)
{
	// Get iterators to elements
	auto iHigh = this->scenes.find(index);
	auto iLow = this->scenes.find(index - 1);

	// Get values
	auto highValue = iHigh->second;
	auto lowValue = iLow->second;

	// Remove both elements
	this->scenes.erase(index);
	this->scenes.erase(index - 1);

	// Put elements back in reverse
	this->scenes[index - 1] = highValue;
	this->scenes[index] = lowValue;

	// Go through all dialogues and change their custom next scenes and dialogues
	for (unsigned i = 0; i < this->scenes.size(); i++) {
		for (unsigned j = 0; j < this->scenes[i].second.size(); j++) {
			// If has custom next scene/dialogue
			std::string next = this->scenes[i].second[j]->nextDialogue;
			if (next != "") {
				// If next is in other scene
				if (next.find("otherscene") != std::string::npos) {
					int nextScene = stoi(next.substr(0, next.find("otherscene")));

					if (nextScene == index) {
						nextScene--;
					}
					else if (nextScene == index - 1) {
						nextScene++;
					}

					this->scenes[i].second[j]->nextDialogue = std::to_string(nextScene) + "otherscene";
				}
			}
		}
	}
}


void SceneManager::moveSceneDown(int index)
{
	// Get iterators to elements
	auto iHigh = this->scenes.find(index + 1);
	auto iLow = this->scenes.find(index);

	// Get values
	auto highValue = iHigh->second;
	auto lowValue = iLow->second;

	// Remove both elements
	this->scenes.erase(index);
	this->scenes.erase(index + 1);

	// Put elements back in reverse
	this->scenes[index + 1] = lowValue;
	this->scenes[index] = highValue;

	// Go through all dialogues and change their custom next scenes and dialogues
	for (unsigned i = 0; i < this->scenes.size(); i++) {
		for (unsigned j = 0; j < this->scenes[i].second.size(); j++) {
			// If has custom next scene/dialogue
			std::string next = this->scenes[i].second[j]->nextDialogue;
			if (next != "") {
				// If next is in other scene
				if (next.find("otherscene") != std::string::npos) {
					int nextScene = stoi(next.substr(0, next.find("otherscene")));

					if (nextScene == index) {
						nextScene++;
					}
					else if (nextScene == index + 1) {
						nextScene--;
					}

					this->scenes[i].second[j]->nextDialogue = std::to_string(nextScene) + "otherscene";
				}
			}
		}
	}
}


void SceneManager::moveDialogueUp(int sceneIdx, int dlgIndex)
{
	auto dialogues = this->scenes[sceneIdx].second;

	auto otherDlg = dialogues[dlgIndex - 1];

	dialogues[dlgIndex - 1] = dialogues[dlgIndex];
	dialogues[dlgIndex] = otherDlg;

	for (unsigned i = 0; i < dialogues.size(); i++) {
		std::string next = dialogues[i]->nextDialogue;
		if (next != "") {
			// If custom next is in the same scene
			if (next.find("otherscene") == std::string::npos) {
				int idx = stoi(next);

				if (idx == dlgIndex) {
					idx--;
				}
				else if (idx == dlgIndex - 1) {
					idx++;
				}

				dialogues[i]->nextDialogue = std::to_string(idx);
			}
		}
		else if (dialogues[i]->question) {
			auto dlg = dialogues[i];

			if (dlg->option1Next != "" && dlg->option1Next != "Not Selected") {
				int idx = stoi(dlg->option1Next);

				if (idx == dlgIndex) {
					idx--;
				}
				else if (idx == dlgIndex - 1) {
					idx++;
				}

				dlg->option1Next = std::to_string(idx);
			}

			if (dlg->option2Next != "" && dlg->option2Next != "Not Selected") {
				int idx = stoi(dlg->option2Next);

				if (idx == dlgIndex) {
					idx--;
				}
				else if (idx == dlgIndex - 1) {
					idx++;
				}

				dlg->option2Next = std::to_string(idx);
			}

			if (dlg->option3Next != "" && dlg->option3Next != "Not Selected") {
				int idx = stoi(dlg->option3Next);

				if (idx == dlgIndex) {
					idx--;
				}
				else if (idx == dlgIndex - 1) {
					idx++;
				}

				dlg->option3Next = std::to_string(idx);
			}

			dialogues[i] = dlg;
		}
	}

	this->scenes[sceneIdx].second = dialogues;
}


void SceneManager::moveDialogueDown(int sceneIdx, int dlgIndex)
{
	auto dialogues = this->scenes[sceneIdx].second;

	auto otherDlg = dialogues[dlgIndex + 1];

	dialogues[dlgIndex + 1] = dialogues[dlgIndex];
	dialogues[dlgIndex] = otherDlg;

	for (unsigned i = 0; i < dialogues.size(); i++) {
		std::string next = dialogues[i]->nextDialogue;
		if (next != "") {
			// If custom next is in the same scene
			if (next.find("otherscene") == std::string::npos) {
				int idx = stoi(next);

				if (idx == dlgIndex) {
					idx++;
				}
				else if (idx == dlgIndex + 1) {
					idx--;
				}

				dialogues[i]->nextDialogue = std::to_string(idx);
			}
		}
		else if (dialogues[i]->question) {
			auto dlg = dialogues[i];

			if (dlg->option1Next != "" && dlg->option1Next != "Not Selected") {
				int idx = stoi(dlg->option1Next);

				if (idx == dlgIndex) {
					idx++;
				}
				else if (idx == dlgIndex + 1) {
					idx--;
				}

				dlg->option1Next = std::to_string(idx);
			}

			if (dlg->option2Next != "" && dlg->option2Next != "Not Selected") {
				int idx = stoi(dlg->option2Next);

				if (idx == dlgIndex) {
					idx++;
				}
				else if (idx == dlgIndex + 1) {
					idx--;
				}

				dlg->option2Next = std::to_string(idx);
			}

			if (dlg->option3Next != "" && dlg->option3Next != "Not Selected") {
				int idx = stoi(dlg->option3Next);

				if (idx == dlgIndex) {
					idx++;
				}
				else if (idx == dlgIndex + 1) {
					idx--;
				}

				dlg->option3Next = std::to_string(idx);
			}

			dialogues[i] = dlg;
		}
	}

	this->scenes[sceneIdx].second = dialogues;
}


void SceneManager::loadFromFile(const std::string& filePath)
{
	YAML::Node file = YAML::LoadFile(filePath);

	int sceneIndex = 0;
	int dialogueIndex = 0;
	std::string currentSceneName = "";
	std::string currentDialogueName = "";

	// While there's more scenes
	while (file[std::to_string(sceneIndex)] != nullptr) {
		// Get current scene's name
		currentSceneName = file[std::to_string(sceneIndex)]["name"].as<std::string>();

		this->addScene(currentSceneName);

		this->sceneBackgrounds.push_back(file[std::to_string(sceneIndex)]["Background"].as<std::string>());

		// While there's more dialogues
		while (file[std::to_string(sceneIndex)][std::to_string(dialogueIndex)] != nullptr) {
			YAML::Node currentDialogue = file[std::to_string(sceneIndex)][std::to_string(dialogueIndex)];

			currentDialogueName = currentDialogue["name"].as<std::string>();

			std::string talking = (currentDialogue["talker"] != nullptr) ? currentDialogue["talker"].as<std::string>() : "";
			std::string left = (currentDialogue["left"] != nullptr) ? currentDialogue["left"].as<std::string>() : "";
			std::string right = (currentDialogue["right"] != nullptr) ? currentDialogue["right"].as<std::string>() : "";
			std::string message = (currentDialogue["message"] != nullptr) ? currentDialogue["message"].as<std::string>() : "";
			std::string next = "";
			bool dialogueBox = (message.length() > 0) ? true : false;
			bool question = false;
			bool talkerBox = (talking.length() > 0) ? true : false;
			std::string askingQuestion = (currentDialogue["asking"] != nullptr) ? currentDialogue["asking"].as<std::string>() : "";

			// Set all question related variables here, because they're set only if the dialogue is a question
			std::string option1Text = "", option1Next = "Not Selected", option2Text = "", option2Next = "Not Selected", option3Text = "", option3Next = "Not Selected";
			int option1Influence = 1, option2Influence = 1, option3Influence = 1;

			// Set next dialogue or next scene
			if (currentDialogue["next"] != nullptr && currentDialogue["custom_next"] != nullptr)
				next = currentDialogue["next"].as<std::string>();
			else if (currentDialogue["next_scene"] != nullptr && currentDialogue["custom_next"] != nullptr)
				next = currentDialogue["next_scene"].as<std::string>() + "otherscene";

			// Question related
			if (currentDialogueName.find("Question") != std::string::npos) {
				question = true;
				dialogueBox = false;
				talkerBox = false;

				option1Text = currentDialogue["Option 1"]["text"].as<std::string>();
				option1Influence = currentDialogue["Option 1"]["influence"].as<int>();
				option1Next = currentDialogue["Option 1"]["next"].as<std::string>();

				option2Text = currentDialogue["Option 2"]["text"].as<std::string>();
				option2Influence = currentDialogue["Option 2"]["influence"].as<int>();
				option2Next = currentDialogue["Option 2"]["next"].as<std::string>();

				option3Text = currentDialogue["Option 3"]["text"].as<std::string>();
				option3Influence = currentDialogue["Option 3"]["influence"].as<int>();
				option3Next = currentDialogue["Option 3"]["next"].as<std::string>();
			}

			Dialogue* dialogue = new Dialogue(
				dialogueIndex,
				currentDialogueName,
				talking,
				left,
				right,
				message,
				next,
				dialogueBox,
				question,
				talkerBox,
				askingQuestion,

				option1Text,
				option1Influence,
				option1Next,

				option2Text,
				option2Influence,
				option2Next,

				option3Text,
				option3Influence,
				option3Next
			);

			this->addDialogue(sceneIndex, dialogue);

			dialogueIndex++;
		}

		dialogueIndex = 0;
		sceneIndex++;
	}
}


void SceneManager::saveToFile(const std::string& filePath)
{
	std::ofstream file(filePath);

	if (file.is_open()) {
		std::cout << "Saving...\n";

		for (unsigned i = 0; i < this->scenes.size(); i++) {
			file << i << ":\n";
			file << "    " << "name: " << this->scenes[i].first << "\n\n";

			// If background is not set
			if (this->sceneBackgrounds.size() <= i)
				file << "    " << "Background: " << "BACKGROUND NOT SET" << "\n\n";
			else
				file << "    " << "Background: " << this->sceneBackgrounds[i] << "\n\n";

			std::vector<Dialogue *> dialogues = this->scenes[i].second;

			for (unsigned j = 0; j < dialogues.size(); j++) {
				file << "    " << dialogues[j]->index << ":\n";
				file << "        " << "name: " << dialogues[j]->name << "\n";

				// A normal dialogue
				if (!dialogues[j]->question) {
					if (dialogues[j]->talking != "" && dialogues[j]->talker)
						file << "        " << "talker: " << dialogues[j]->talking << "\n";

					if (dialogues[j]->left != "")
						file << "        " << "left: " << dialogues[j]->left << "\n";

					if (dialogues[j]->right != "")
						file << "        " << "right: " << dialogues[j]->right << "\n";

					if (dialogues[j]->showTextBox && dialogues[j]->message != "")
						file << "        " << "message: " << dialogues[j]->message << "\n";

					// Default next
					if (dialogues[j]->nextDialogue == "") {
						if (j != dialogues.size() - 1)
							file << "        " << "next: " << j + 1 << "\n";
						else if (i != this->scenes.size() - 1)
							file << "        " << "next_scene: " << i + 1 << "\n";
					}
					// Custom next
					else {
						std::string ndlg = dialogues[j]->nextDialogue;

						// If other scene
						if (ndlg.find("otherscene") != std::string::npos) {
							file << "        " << "next_scene: " << ndlg.substr(0, ndlg.find("otherscene")) << "\n";
						}
						else {
							file << "        " << "next: " << ndlg << "\n";
						}

						// Add custom next flag
						file << "        " << "custom_next: true\n";

						file << "\n";
					}
				}
				// A question
				else {
					if (dialogues[j]->charAskingQuestion != "")
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
