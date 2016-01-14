#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <map>

class Card;
class Player;
class Character;
enum class EnumState;
enum class EnumCharacter;

class Game
{
public:
	Game();
	~Game();
	std::map<EnumCharacter, std::shared_ptr<Character>> getCharactersDeck() const { return charactersDeck_; }
	void addPlayer(std::shared_ptr<Player> player);
	std::shared_ptr<Player> getPlayerOnIndex(int index) const { return players_.at(index%2); }
	void switchState(EnumState state);
	EnumState getCurrentState() const { return currentState_; }
	std::string getPlayerName(int turnCounter);
	std::pair<EnumCharacter, std::shared_ptr<Character>> removeCharacter(EnumCharacter character);
	bool moveCharacterFromDecktoPlayer(EnumCharacter character, std::shared_ptr<Player>);
private:
	void createBuildingCards();
	void createCharacterCards();
	std::vector<std::shared_ptr<Card>> buildingsDeck_;
	std::map<EnumCharacter, std::shared_ptr<Character>> charactersDeck_;
	std::vector<std::shared_ptr<Player>> players_;
	std::vector<std::string> split(std::string& s, char delim);
	EnumState currentState_;
};

