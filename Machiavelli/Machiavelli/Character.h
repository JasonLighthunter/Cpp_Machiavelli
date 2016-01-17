#pragma once

enum class EnumCharacter;

class Character
{
public:
	Character(EnumCharacter role);
	~Character();

	bool isMurdered() { return isMurdered_; }
	void setMurdered(bool b);

	bool abilityUsed() { return abilityUsed_; }
	void setAbilityUsed(bool b);

	bool alreadyGetGoldOrBuilding() const { return getGoldOrBuilding_; }
	void setGoldOrBuilding(bool gob) { getGoldOrBuilding_ = gob; }
private:
	EnumCharacter role_;
	bool isMurdered_ = false;
	bool abilityUsed_= false;
	bool getGoldOrBuilding_ = false;
};
