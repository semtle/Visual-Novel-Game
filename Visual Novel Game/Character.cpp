#include "Character.h"

Character::Character(const std::string& name, float Influence)
{
	this->name = name;
	this->influence = Influence;

	std::string line;
	std::ifstream file("SaveFiles/CharacterPaths/paths.txt");
	if (file.is_open()) {
		while (getline(file, line)) {
			this->images.emplace(line, Bengine::ResourceManager::getTexture("Textures/Characters/" + name + "/" + line + ".png").id);
		}
		file.close();
		std::cout << "Read file successfully.\n";
	}
	else std::cout << "Unable to open character image path file!\n";
}


Character::~Character()
{
}