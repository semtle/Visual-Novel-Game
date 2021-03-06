#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <iterator>

#include <Bengine/Bengine.h>
#include <Bengine/Window.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteFont.h>

struct Dialogue {
	Dialogue() :
		index(-1),
		name(""),
		talking(""),
		left(""),
		right(""),
		message(""),
		nextDialogue(""),
		showTextBox(true),
		question(false),
		talker(false),
        fadeIn(false),
        fadeOut(false),
        startMusic(""),
        soundEffect(""),
        endMusic(false),
        boxColor(1),

		charAskingQuestion(""),

		option1Text(""),
		option1Influence(1),
		option1Next(""),

		option2Text(""),
		option2Influence(1),
		option2Next(""),

		option3Text(""),
		option3Influence(1),
		option3Next("")
	{ }

	Dialogue(
		int Index,
		std::string Name,
		std::string Talking,
		std::string Left,
		std::string Right,
		std::string Message,
		std::string NextDialogue,
		bool ShowTextBox,
		bool Question,
		bool Talker,
        bool FadeIn,
        bool FadeOut,
        std::string StartMusic,
        std::string SoundEffect,
        bool EndMusic,
        int BoxColor,

		std::string CharAskingQuestion,

		std::string Option1Text,
		int Option1Influence,
		std::string Option1Next,

		std::string Option2Text,
		int Option2Influence,
		std::string Option2Next,

		std::string Option3Text,
		int Option3Influence,
		std::string Option3Next
	) :
		index(Index),
		name(Name),
		talking(Talking),
		left(Left),
		right(Right),
		message(Message),
		nextDialogue(NextDialogue),
		showTextBox(ShowTextBox),
		question(Question),
		talker(Talker),
        fadeIn(FadeIn),    
        fadeOut(FadeOut),
        startMusic(StartMusic),
        soundEffect(SoundEffect),
        endMusic(EndMusic),
        boxColor(BoxColor),

		charAskingQuestion(CharAskingQuestion),

		option1Text(Option1Text),
		option1Influence(Option1Influence),
		option1Next(Option1Next),

		option2Text(Option2Text),
		option2Influence(Option2Influence),
		option2Next(Option2Next),

		option3Text(Option3Text),
		option3Influence(Option3Influence),
		option3Next(Option3Next)
	{ }

	int index;
	std::string name;
	std::string talking;
	std::string left;
	std::string right;
	std::string message;
	std::string nextDialogue;

	bool showTextBox;
	bool question;
	bool talker;
    bool fadeIn;
    bool fadeOut;
    std::string startMusic;
    bool endMusic;
    std::string soundEffect;

    // 0 = Red, 1 = Blue, 2 = Green
    int boxColor;

	std::string charAskingQuestion;

	std::string option1Text;
	int option1Influence;
	std::string option1Next;

	std::string option2Text;
	int option2Influence;
	std::string option2Next;

	std::string option3Text;
	int option3Influence;
	std::string option3Next;
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void update();

	void addScene(std::string sceneName);
	void addDialogue(int idx, Dialogue *);

	void loadFromFile(const std::string& filePath);
	void saveToFile(const std::string& filePath);

	// Setters
	void setSceneBackgrounds(std::vector<std::string> bgs) { this->sceneBackgrounds = bgs; }
	void setSceneName(int index, std::string name) { this->scenes[index].first = name; }
	void moveSceneUp(int index);
	void moveSceneDown(int index);
	void moveDialogueUp(int sceneIdx, int dlgIndex);
	void moveDialogueDown(int sceneIdx, int dlgIndex);
	void removeDialogue(int sceneIdx, int dlgIndex);
	void removeScene(int index);

	// Getters
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> getScenes() const { return this->scenes; }
	std::vector<std::string> getSceneBackgrounds() const { return this->sceneBackgrounds; }
	std::vector<Dialogue *> getDialogues(int idx) { return this->scenes[idx].second; }
	int getScenesSize() { return this->scenes.size(); }
	
private:
	std::map<int, std::pair<std::string, std::vector<Dialogue *>>> scenes;
	std::vector<std::string> sceneBackgrounds;
};