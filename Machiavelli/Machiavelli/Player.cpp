#include <algorithm>
#include "Card.h"
#include "Player.hpp"
#include "EnumCard.h"
#include "Character.h"
#include "EnumCharacter.h"

using namespace std;

struct FindStruct {
	EnumCard cardType;

	FindStruct(EnumCard cardType) {
		this->cardType = cardType;
	}

	bool operator()(pair<const int, shared_ptr<Card>> const& m) const {
		Range range = EnumCardToRange.at(cardType);
		return m.first >= range.first && m.first <= range.last;
	}
};

void Player::setIsKing(bool b) {
	isKing_ = b;
}

void Player::setHand(map<int, shared_ptr<Card>> hand) {
	hand_ = hand;
}


void Player::addBuildingCard(std::shared_ptr<Card> card) {
	hand_[card->getId()] = card;
}

void Player::addBuildingToBuildings(std::shared_ptr<Card> card) {
	buildings_[card->getId()] = card;
}

void Player::emptyCurrentRoles() {
	currentRoles_.clear();
}

void Player::addRole(pair<EnumCharacter, shared_ptr<Character>> newRole) {
	currentRoles_.emplace(newRole);
	if(newRole.first==EnumCharacter::KING) {
		isKing_=true;
	}
}

vector<pair<EnumCharacter, shared_ptr<Character>>> Player::getRoles() {
	return vector<pair<EnumCharacter, shared_ptr<Character>>>(currentRoles_.begin(), currentRoles_.end());
}

bool Player::hasRole(EnumCharacter character) {
	for(pair<EnumCharacter, shared_ptr<Character>> role : getRoles()) {
		if(character == role.first) {
			if(role.second->isMurdered()) {
				return false;
			}
			return true;
		}
	}
	return false;
}

bool Player::buildBuilding(string buildingName) {
	auto a = convertToEnumCard.find(buildingName);
	
	if (a == convertToEnumCard.end()) {
		return false;
	}
	
	EnumCard enumCard = a->second;

	auto result = find_if(hand_.begin(), hand_.end(), FindStruct(enumCard));
	if (result != hand_.end()) {
		auto card = result->second;
		if (card->getCosts() <= gold_) {
			buildings_[card->getId()] = card;
			hand_.erase(card->getId());
			increaseGold(-card->getCosts());
			return true;
		}
	}
	return false;
}

void Player::destroyBuilding(pair<const int, std::shared_ptr<Card>> building) {
	buildings_.erase(building.first);
}

void Player::murderRole(EnumCharacter character) {
	for(pair<EnumCharacter, shared_ptr<Character>> role : currentRoles_) {
		if(role.first == character) {
			role.second->setMurdered(true);
			return;
		}
	}
}
EnumCharacter Player::getMurderTarget() {
	for(pair<EnumCharacter, shared_ptr<Character>> role:currentRoles_) {
		if(role.second->isMurdered()) {
			return role.first;
		}
	}
	return EnumCharacter::ASSASSIN;
}
void Player::markRoleForTheft(EnumCharacter character) {
	for(pair<EnumCharacter, shared_ptr<Character>> role:currentRoles_) {
		if(role.first == character) {
			role.second->isMarkedForTheft(true);
			return;
		}
	}
}

bool Player::isMarkedForTheft(EnumCharacter character) {
	for(pair<EnumCharacter, shared_ptr<Character>> role:currentRoles_) {
		if(role.second->isMarkedForTheft()) {
			return true;
		}
	}
	return false;
}
