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
	Character(const std::string& name, float Influence = 0.0f);
	~Character();

	std::map<std::string, int> const getImages() { return this->images; }

private:
	std::string name;
	float influence;
	std::map<std::string, int> images;
};