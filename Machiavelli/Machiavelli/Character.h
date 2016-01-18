#pragma once

enum class EnumCharacter;

class Character
{
public:
	Character(EnumCharacter role);
	~Character();

	bool isMurdered() { return isMurdered_; }
	void setMurdered(bool b);

	bool isMarkedForTheft() { return isMarkedForTheft_; }
	void isMarkedForTheft(bool b);

	bool abilityUsed() { return abilityUsed_; }
	void setAbilityUsed(bool b);

	bool alreadyGetGoldOrBuilding() const { return getGoldOrBuilding_; }
	void setGoldOrBuilding(bool gob) { getGoldOrBuilding_ = gob; }

	bool buildedBuilding() const { return buildedBuilding_; }
	void setBuildedBuilding(bool bb) { buildedBuilding_ = bb; }
private:
	EnumCharacter role_;
	bool isMurdered_ = false;
	bool isMarkedForTheft_ = false;
	bool abilityUsed_= false;
	bool getGoldOrBuilding_ = false;
	bool buildedBuilding_ = false;
};
