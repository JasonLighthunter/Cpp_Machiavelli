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
			return true;
		}
	}
	return false;
}

bool Player::buildBuilding(string buildingName) {
	EnumCard enumCard = convertToEnumCard.at(buildingName);

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
