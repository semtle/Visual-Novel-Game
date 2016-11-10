#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>

#include <Bengine/ResourceManager.h>

class Character
{
public:
	Character(const std::string& name, int Influence = 0);
	~Character();

	// Setters
	void addInfluence(int amount) { this->influence += amount; }

	// Getters
	int getInfluence() const { return this->influence; }
	std::map<std::string, int> const getImages() { return this->images; }
private:
	std::string name;
	int influence;
	std::map<std::string, int> images;
};