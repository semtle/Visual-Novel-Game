#include "Character.h"
#include <SDL/SDL.h>

Character::Character(const std::string& name, int Influence)
{
	this->name = name;
	this->influence = Influence;

	std::string line;
	std::ifstream file("SaveFiles/CharacterPaths/paths.txt");
	if (file.is_open()) {
		Uint32 ticks = SDL_GetTicks();

		while (getline(file, line)) {
			this->images.emplace(line + ".png", Bengine::ResourceManager::getTexture("Textures/Characters/" + name + "/" + line + ".png").id);
		}
		std::cout << "Ticks: " << SDL_GetTicks() - ticks << "\n";
		file.close();
		std::cout << "Read file successfully.\n";
	}
	else std::cout << "Unable to open character image path file!\n";

	// Makoto-kun has some extra images :-)
	if (name == "Makoto-kun") {
		this->images.emplace("bored_uniform.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/bored_uniform.png").id);
		this->images.emplace("bored_freetime.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/bored_freetime.png").id);

		this->images.emplace("smiling_side_uniform.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/smiling_side_uniform.png").id);
		this->images.emplace("smiling_side_freetime.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/smiling_side_freetime.png").id);

		this->images.emplace("confident_uniform.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/confident_uniform.png").id);
		this->images.emplace("confident_freetime.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/confident_freetime.png").id);

		this->images.emplace("shouting_uniform.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/shouting_uniform.png").id);
		this->images.emplace("shouting_freetime.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/shouting_freetime.png").id);

		this->images.emplace("disappointed_uniform.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/disappointed_uniform.png").id);
		this->images.emplace("disappointed_freetime.png", Bengine::ResourceManager::getTexture("Textures/Characters/Makoto-kun/disappointed_freetime.png").id);
	}

	// Teemu-kun also has something extra ;-)
	if (name == "Teemu-kun") {
		this->images.emplace("happy_uniform.png", Bengine::ResourceManager::getTexture("Textures/Characters/Teemu-kun/happy_uniform.png").id);
		this->images.emplace("happy_freetime.png", Bengine::ResourceManager::getTexture("Textures/Characters/Teemu-kun/happy_freetime.png").id);
	}
}


Character::~Character()
{
}