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
#include "EnumTurnState.h"

class Card;
class Character;
enum class EnumCharacter;

class Player {
public:
	Player() : currentTurnState_{ EnumTurnState::DEFAULT } {}
	Player(const std::string& name) : name_ {name}, currentTurnState_{ EnumTurnState::DEFAULT } {}

	std::string getName() const { return name_; }
	void setName(const std::string& new_name) { name_ = new_name; }

	void increaseGold(const int increase_with) { gold_ += increase_with; }
	void decreaseGold(const int decrease_with) { gold_ -= decrease_with; }
	int getGold() const { return gold_; }
	bool isKing() const { return isKing_; }
	void setIsKing(bool b);

	std::map<int, std::shared_ptr<Card>>& getHand() { return hand_; }
	void setHand(std::map<int, std::shared_ptr<Card>> hand);

	std::map<int, std::shared_ptr<Card>> getBuildings() const { return buildings_; }
	void addBuildingCard(std::shared_ptr<Card> card);
	void addBuildingToBuildings(std::shared_ptr<Card> card);
	void emptyCurrentRoles();
	void addRole(std::pair<EnumCharacter, std::shared_ptr<Character>> newRole);
	std::vector<std::pair<EnumCharacter, std::shared_ptr<Character>>> getRoles();
	std::shared_ptr<Character> getCharacter(EnumCharacter c) { return currentRoles_.at(c); }
	bool hasRole(EnumCharacter);
	EnumTurnState getCurrentTurnState() const { return currentTurnState_; }
	void setCurrentTurnState(EnumTurnState newState) { currentTurnState_ = newState; }
	bool buildBuilding(std::string buildingName);

	bool isFirstComplete() const { return firstComplete_; }
	void setFirstComplete() { firstComplete_ = true; }

	void destroyBuilding(std::pair<const int, std::shared_ptr<Card>> building);

	void murderRole(EnumCharacter character);
	EnumCharacter getMurderTarget();

	void markRoleForTheft(EnumCharacter character);
	bool isMarkedForTheft(EnumCharacter character);
private:
	std::map<int, std::shared_ptr<Card>> hand_;
	std::map<int, std::shared_ptr<Card>> buildings_;
	std::map<EnumCharacter, std::shared_ptr<Character>> currentRoles_;
	std::string name_;
	EnumTurnState currentTurnState_;
	bool isKing_ = false;
	bool firstComplete_ = false;
	int gold_ = 0;
};

#endif /* Player_hpp */
