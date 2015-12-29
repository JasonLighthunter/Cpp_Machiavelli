#pragma once
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
	{ "Moordenaar", EnumCharacter::MOORDENAAR },
	{ "Dief", EnumCharacter::DIEF },
	{ "Magiër", EnumCharacter::MAGIER },
	{ "Koning", EnumCharacter::KONING },
	{ "Prediker", EnumCharacter::PREDIKER },
	{ "Koopman", EnumCharacter::KOOPMAN },
	{ "Bouwmeester", EnumCharacter::BOUWMEESTER },
	{ "Condottiere", EnumCharacter::CONDOTTIERE }
};