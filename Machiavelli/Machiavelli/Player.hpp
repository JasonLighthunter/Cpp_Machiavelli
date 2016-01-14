//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "Character.h"

enum class EnumCharacter;
enum class EnumCard;
class Card;

class Player {
public:
	Player() {}
	Player(const std::string& name) : name {name} {}
	
	std::string getName() const { return name; }
	void setName(const std::string& new_name) { name = new_name; }

	void increaseGold(const int increase_with) { gold += increase_with; }

	bool BuildBuilding(std::string buildingName);
private:
	std::map<int, std::shared_ptr<Card>> hand_;
	std::map<int, std::shared_ptr<Card>> buildings;
	std::map<EnumCharacter, std::unique_ptr<Character>> currentRoles;
	std::string name;
	int gold = 0;
};

#endif /* Player_hpp */
