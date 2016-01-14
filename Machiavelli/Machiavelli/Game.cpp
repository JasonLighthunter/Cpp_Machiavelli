#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
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

void Game::init() {
	random_device dev;
	default_random_engine dre{ dev() };

	for (auto player : players_) {
		player->increaseGold(2);

		while (player->getHand().size() < 4) {
			uniform_int_distribution<int> dist{ 1, static_cast<int>(buildingsDeck_.size()) - 1 };
			int pos = dist(dre);
			auto card = buildingsDeck_.at(pos);
			player->addBuildingCard(card);
			buildingsDeck_.erase(buildingsDeck_.begin() + pos);
		}
	}
}

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
	return returnValue;
}

bool Game::moveCharacterFromDecktoPlayer(EnumCharacter character, shared_ptr<Player> player) {
	try{
		player->addRole(removeCharacter(character));
	} catch(...) {
		cerr<<convertFromEnumCharacter.at(character)<<" is not in the characterdeck.\n\r";
		return false;
	}
	return true;
}

void Game::createBuildingCards() {
	vector<string> lines;
	string textfile{ "../Bouwkaarten.csv" };
	ifstream input_file{ textfile };
	if (!input_file) {
		cout << "404: Bouwkaarten.csv not found.\n";
		exit(0);
	}

	int lineNumber = 1;
	string line;
	while (getline(input_file, line)) {
		vector<string> parts = split(line, ';');
		shared_ptr<Card> buildingCard{ new Card{lineNumber, parts[0], stoi(parts[1]), convertToEnumColor.at(parts[2])} };
		buildingsDeck_.push_back(buildingCard);
		lineNumber++;
	}
}

void Game::createCharacterCards() {
	vector<string> lines;
	string textfile{ "../Karakterkaarten.csv" };
	ifstream input_file{ textfile };
	if (!input_file) {
		cout << "404: Karakterkaarten.csv not found.\n";
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
