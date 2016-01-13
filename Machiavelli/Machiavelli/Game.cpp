#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Game.h"
#include "Card.h"
#include "Character.h"
#include "EnumCharacter.h"
#include "EnumColor.h"

using namespace std;

Game::Game()
{
	createBuildingCards();
	createCharacterCards();
}


Game::~Game()
{
}

void Game::addPlayer(shared_ptr<Player> player) {
	players_.push_back(player);
}

void Game::switchState(EnumState state) {
	currentState_ = state;
}

void Game::createBuildingCards() {
	vector<string> lines;
	string textfile{ "../Bouwkaarten.csv" };
	ifstream input_file{ textfile };
	if (!input_file) {
		cout << "404: Items text file not found." << endl;
		exit(0);
	}

	string line;
	while (getline(input_file, line)) {
		vector<string> parts = split(line, ';');
		shared_ptr<Card> buildingCard{ new Card{parts[0], stoi(parts[1]), convertToEnumColor.at(parts[2])} };
		buildingsDeck_.push_back(buildingCard);
	}
}

void Game::createCharacterCards() {
	vector<string> lines;
	string textfile{ "../Karakterkaarten.csv" };
	ifstream input_file{ textfile };
	if (!input_file) {
		cout << "404: Items text file not found." << endl;
		exit(0);
	}

	string line;
	while (getline(input_file, line)) {
		vector<string> parts = split(line, ';');
		shared_ptr<Character> buildingCard{ new Character{ stoi(parts[0]), convertToEnumCharacter.at(parts[1]) } };
		charactersDeck_.push_back(buildingCard);
	}
}


vector<string> Game::split(string & s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> elems;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}
