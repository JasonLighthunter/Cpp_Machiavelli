#include "Character.h"
#include "EnumCharacter.h"

Character::Character(EnumCharacter role) {
	role_ = role;
}

Character::~Character() {}

void Character::setMurdered(bool b) {
	isMurdered_ = b;
}

void Character::setAbilityUsed(bool b) {
	abilityUsed_ = b;
}
