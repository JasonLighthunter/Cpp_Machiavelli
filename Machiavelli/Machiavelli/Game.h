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
	void createBuildingCards();
	void createCharacterCards();
	std::vector<std::string> split(std::string& s, char delim);
	std::vector<std::shared_ptr<Card>> buildingsDeck;
	std::vector<std::shared_ptr<Character>> charactersDeck;
};

