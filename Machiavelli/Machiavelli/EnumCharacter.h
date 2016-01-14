#pragma once
#include <map>

enum class EnumCharacter {
	MOORDENAAR,
	DIEF,
	MAGIER,
	KONING,
	PREDIKER,
	KOOPMAN,
	BOUWMEESTER,
	CONDOTTIERE
};

const std::map<std::string, EnumCharacter> convertToEnumCharacter = {
	{ "moordenaar", EnumCharacter::MOORDENAAR },
	{ "dief", EnumCharacter::DIEF },
	{ "magier", EnumCharacter::MAGIER },
	{ "koning", EnumCharacter::KONING },
	{ "prediker", EnumCharacter::PREDIKER },
	{ "koopman", EnumCharacter::KOOPMAN },
	{ "bouwmeester", EnumCharacter::BOUWMEESTER },
	{ "condottiere", EnumCharacter::CONDOTTIERE }
};

const std::map<EnumCharacter, std::string> convertFromEnumCharacter = {
	{EnumCharacter::MOORDENAAR,"moordenaar"},
	{EnumCharacter::DIEF, "dief"},
	{EnumCharacter::MAGIER, "magier"},
	{EnumCharacter::KONING, "koning"},
	{EnumCharacter::PREDIKER, "prediker"},
	{EnumCharacter::KOOPMAN, "koopman"},
	{EnumCharacter::BOUWMEESTER, "bouwmeester"},
	{EnumCharacter::CONDOTTIERE, "condottiere"}
};