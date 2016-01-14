#pragma once
#include <map>

enum class EnumCharacter {
	ASSASSIN,
	THIEF,
	MAGICIAN,
	KING,
	BISHOP,
	MERCHANT,
	ARCHITECT,
	WARLORD
};

const std::map<std::string, EnumCharacter> convertToEnumCharacter = {
	{ "moordenaar", EnumCharacter::ASSASSIN },
	{ "dief", EnumCharacter::THIEF },
	{ "magier", EnumCharacter::MAGICIAN },
	{ "koning", EnumCharacter::KING },
	{ "prediker", EnumCharacter::BISHOP },
	{ "koopman", EnumCharacter::MERCHANT },
	{ "bouwmeester", EnumCharacter::ARCHITECT },
	{ "condottiere", EnumCharacter::WARLORD }
};

const std::map<EnumCharacter, std::string> convertFromEnumCharacter = {
	{EnumCharacter::ASSASSIN,"moordenaar"},
	{EnumCharacter::THIEF, "dief"},
	{EnumCharacter::MAGICIAN, "magier"},
	{EnumCharacter::KING, "koning"},
	{EnumCharacter::BISHOP, "prediker"},
	{EnumCharacter::MERCHANT, "koopman"},
	{EnumCharacter::ARCHITECT, "bouwmeester"},
	{EnumCharacter::WARLORD, "condottiere"}
};
