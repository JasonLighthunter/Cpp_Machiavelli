#pragma once
#include <map>
#include <memory>
#include <vector>

enum class EnumCharacter;
class Character;
class Card;

class Game
{
public:
	Game();
	~Game();
private:
	std::vector<std::unique_ptr<Card>> deck;
	std::map<EnumCharacter, std::unique_ptr<Character>> allRoles;
};

