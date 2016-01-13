//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <vector>
#include <map>
#include "Card.h"
#include "Character.h"

enum class EnumCharacter;

class Player {
public:
	Player() {}
	Player(const std::string& name) : name_ {name} {}

	std::string getName() const { return name_; }
	void setName(const std::string& new_name) { name_ = new_name; }

	int getGold() const { return gold_; }
	bool isKing() const { return isKing_; }
private:
	std::vector<std::unique_ptr<Card>> hand;
	std::vector<std::unique_ptr<Card>> buildings;
	std::map<EnumCharacter, std::unique_ptr<Character>> currentRoles;
	std::string name_;
	bool isKing_ = false;
	int gold_ = 0;
};

#endif /* Player_hpp */
