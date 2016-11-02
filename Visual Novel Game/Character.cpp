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

	// Makoto-kun has some extra images :-)
	if (name == "Makoto-kun") {
		this->images.emplace("bored_uniform", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/bored_uniform.png").id);
		this->images.emplace("bored_freetime", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/bored_freetime.png").id);

		this->images.emplace("smiling_side_uniform", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/smiling_side_uniform.png").id);
		this->images.emplace("smiling_side_freetime", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/smiling_side_freetime.png").id);

		this->images.emplace("confident_uniform", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/confident_uniform.png").id);
		this->images.emplace("confident_freetime", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/confident_freetime.png").id);

		this->images.emplace("shouting_uniform", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/shouting_uniform.png").id);
		this->images.emplace("shouting_freetime", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/shouting_freetime.png").id);

		this->images.emplace("disappointed_uniform", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/disappointed_uniform.png").id);
		this->images.emplace("disappointed_freetime", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/disappointed_freetime.png").id);
	}

	// Teemu-kun also has something extra ;-)
	if (name == "Teemu-kun") {
		this->images.emplace("happy_uniform", Bengine::ResourceManager::getTexture("Textures/Characters/Teemu-kun/happy_uniform.png").id);
		this->images.emplace("happy_freetime", Bengine::ResourceManager::getTexture("Textures/Characters/Teemu-kun/happy_freetime.png").id);
	}
}


Character::~Character()
{
}