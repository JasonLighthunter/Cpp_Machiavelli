#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Game.h"
#include "Card.h"
#include "Character.h"
#include "EnumCharacter.h"
#include "EnumColor.h"
#include "Player.hpp"

using namespace std;

Game::Game() {
	createBuildingCards();
	createCharacterCards();
}

Game::~Game() {}

void Game::addPlayer(shared_ptr<Player> player) {
	players_.push_back(player);
}

void Game::switchState(EnumState state) {
	currentState_ = state;
}

std::string Game::getPlayerName(int turnCounter) {
	return players_.at(turnCounter%2)->getName();
}

pair<EnumCharacter, shared_ptr<Character>> Game::removeCharacter(EnumCharacter character) {
	pair<EnumCharacter, shared_ptr<Character>> returnValue = {character, charactersDeck_.at(character)};
	charactersDeck_.erase(character);
	cout<<"thrown out: "<<convertFromEnumCharacter.at(character);
	return returnValue;
}

bool Game::moveCharacterFromDecktoPlayer(EnumCharacter character, shared_ptr<Player> player) {
	try{
		player->addRole(removeCharacter(character));
	} catch(...) {
		cerr<<convertFromEnumCharacter.at(character)<<" is not in the characterdeck.\n\r";
		return false;
	}
	cout<<player->getName()<<" has: "<<convertFromEnumCharacter.at(character)<<"\n\r";
	return true;
}

void Game::createBuildingCards() {
	vector<string> lines;
	string textfile{ "../Bouwkaarten.csv" };
	ifstream input_file{ textfile };
	if (!input_file) {
		cout << "404: Items text file not found.\n";
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
		cout << "404: Items text file not found.\n";
		exit(0);
	}

	string line;
	while(getline(input_file, line)) {
		vector<string> parts=split(line, ';');
		shared_ptr<Character> buildingCard{new Character{stoi(parts[0]), convertToEnumCharacter.at(parts[1])}};
		charactersDeck_.emplace(convertToEnumCharacter.at(parts[1]), buildingCard);
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
