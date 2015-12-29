#pragma once

enum class EnumCharacter;

class Character
{
public:
	Character(int priority, EnumCharacter role);
	~Character();
private:
	EnumCharacter role_;
	int priority_;
};

