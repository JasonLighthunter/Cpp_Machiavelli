#pragma once
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
	{ "Moordenaar", EnumCharacter::ASSASSIN },
	{ "Dief", EnumCharacter::THIEF },
	{ "Magiër", EnumCharacter::MAGICIAN },
	{ "Koning", EnumCharacter::KING },
	{ "Prediker", EnumCharacter::BISHOP },
	{ "Koopman", EnumCharacter::MERCHANT },
	{ "Bouwmeester", EnumCharacter::ARCHITECT },
	{ "Condottiere", EnumCharacter::WARLORD }
};