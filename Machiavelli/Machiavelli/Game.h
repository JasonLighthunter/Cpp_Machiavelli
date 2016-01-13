#pragma once

#include <vector>
#include <memory>

class Card;
class Player;
class Character;
enum class EnumState;

class Game
{
public:
	Game();
	~Game();
	std::vector<std::shared_ptr<Character>> GetCharactersDeck() const { return charactersDeck_; }
	void addPlayer(std::shared_ptr<Player> player);
	std::shared_ptr<Player> getPlayerOnIndex(int index) const { return players_.at(index); }
	void switchState(EnumState state);
	EnumState getCurrentState() const { return currentState_; }
private:
	void createBuildingCards();
	void createCharacterCards();
	std::vector<std::shared_ptr<Card>> buildingsDeck_;
	std::vector<std::shared_ptr<Character>> charactersDeck_;
	std::vector<std::shared_ptr<Player>> players_;
	std::vector<std::string> split(std::string& s, char delim);
	EnumState currentState_;
};

