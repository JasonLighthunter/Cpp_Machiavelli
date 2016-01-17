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
#include <memory>

class Card;
class Character;
enum class EnumCharacter;

class Player {
public:
	Player() {}
	Player(const std::string& name) : name_ {name} {}

	std::string getName() const { return name_; }
	void setName(const std::string& new_name) { name_ = new_name; }

	void increaseGold(const int increase_with) { gold_ += increase_with; }
	int getGold() const { return gold_; }
	bool isKing() const { return isKing_; }
	void setIsKing(bool b);

	std::map<int, std::shared_ptr<Card>> getHand() const { return hand_; }
	void addBuildingCard(std::shared_ptr<Card> card);
	void emptyCurrentRoles();
	void addRole(std::pair<EnumCharacter, std::shared_ptr<Character>> newRole);
	std::vector<std::pair<EnumCharacter, std::shared_ptr<Character>>> getRoles();
	bool hasRole(EnumCharacter);

	bool buildBuilding(std::string buildingName);

	void murderRole(EnumCharacter character);
private:
	std::map<int, std::shared_ptr<Card>> hand_;
	std::map<int, std::shared_ptr<Card>> buildings_;
	std::map<EnumCharacter, std::shared_ptr<Character>> currentRoles_;
	std::string name_;

	bool isKing_ = false;
	int gold_ = 0;
};

#endif /* Player_hpp */
