#include <algorithm>
#include "Card.h"
#include "Player.hpp"
#include "EnumCard.h"
#include "Character.h"

using namespace std;

struct FindStruct {
	EnumCard cardType;

	FindStruct(EnumCard cardType) {
		this->cardType = cardType;
	}

	bool operator()(pair<const int, std::shared_ptr<Card>> const& m) const {
		Range range = EnumCardToRange.at(cardType);

		return m.first >= range.first && m.first <= range.last;
	}
};

bool Player::BuildBuilding(std::string buildingName)
{
	EnumCard enumCard = convertToEnumCard.at(buildingName);

	auto result = std::find_if(hand_.begin(), hand_.end(), FindStruct(enumCard));
	if (result != hand_.end()) {
		auto card = result->second;
		if (card->getCosts() <= gold) {
			buildings[card->getId()] = card;
			hand_.erase(card->getId());
			increaseGold(-card->getCosts());
			return true;
		}
	}
	return false;
}
