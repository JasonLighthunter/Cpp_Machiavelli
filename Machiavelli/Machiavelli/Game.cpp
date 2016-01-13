#include <sstream>
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include "Game.h"
#include "Card.h"
#include "Character.h"
#include "EnumColor.h"
#include "EnumCharacter.h"

using namespace std;

Game::Game()
{
	createBuildingCards();
	createCharacterCards();
}


Game::~Game()
{
}


void Game::createBuildingCards() {
	vector<string> lines;
	string textfile{ "../Bouwkaarten.csv" };
	ifstream input_file{ textfile };
	if (!input_file) {
		cout << "404: Items text file not found." << endl;
		exit(0);
	}

	int lineNumber = 1;
	string line;
	while (getline(input_file, line)) {
		vector<string> parts = split(line, ';');
		shared_ptr<Card> buildingCard{ new Card{lineNumber, parts[0], stoi(parts[1]), convertToEnumColor.at(parts[2])} };
		buildingsDeck.push_back(buildingCard);
		lineNumber++;
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
		charactersDeck.push_back(buildingCard);
	}
}


vector<string> Game::split(string & s, char delim) {
	stringstream ss(s);
	string item;
	vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}
