#pragma once

enum class EnumCharacter;

class Character
{
public:
	Character(int priority, EnumCharacter role);
	~Character();
	bool alreadyGetGoldOrBuilding() const { return getGoldOrBuilding_; }
	void setGoldOrBuilding(bool gob) { getGoldOrBuilding_ = gob; }
private:
	EnumCharacter role_;
	int priority_;
	bool getGoldOrBuilding_ = false;
};

