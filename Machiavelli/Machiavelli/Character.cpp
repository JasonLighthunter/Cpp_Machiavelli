#include "Character.h"
#include "EnumCharacter.h"

Character::Character(int priority, EnumCharacter role)
{
	priority_ = priority;
	role_ = role;
}

Character::~Character()
{
}
