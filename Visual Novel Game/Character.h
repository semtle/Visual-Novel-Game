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

private:
	std::string name;
	float influence;
	std::map<std::string, unsigned int> images;
};