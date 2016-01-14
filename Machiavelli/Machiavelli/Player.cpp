#include "Player.hpp"
#include "Character.h"
#include "EnumCharacter.h"

using namespace std;

void Player::setIsKing(bool b) {
	isKing_ = b;
}

void Player::addRole(pair<EnumCharacter, shared_ptr<Character>> newRole) {
	currentRoles.emplace(newRole);
}
