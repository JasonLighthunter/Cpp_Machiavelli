#pragma once
#include <queue>
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

	void init();
	void initQuickStart();

	void addPlayer(std::shared_ptr<Player> player);
	std::vector<std::shared_ptr<Player>> getPlayers() const { return players_; }
	std::shared_ptr<Player> getPlayerOnIndex(int turncounter) const { return players_.at(turncounter % 2); }
	std::string getPlayerName(int turnCounter);
	std::shared_ptr<Player> getPlayerWithRole(EnumCharacter character);

	int getIndexOfPlayer(std::shared_ptr<Player> player);

	bool usingAbility() { return usingAbility_; }
	void setUsingAbility(bool b);

	bool goldStolen() { return goldStolen_; }
	void setGoldStolen(bool b);

	std::shared_ptr<Player> getEnemy(std::shared_ptr<Player> currentPlayer);

	bool abilityUsed(EnumCharacter character);
	void setAbilityUsed(bool b, EnumCharacter character);

	int getIndexOfKing();
	void resetGameToSetup();

	void switchState(EnumState state);
	EnumState getCurrentState() const { return currentState_; }
	std::vector<std::shared_ptr<Card>>& getDrawnCards() { return drawnCards_; }
	void drawCards(int amount);
	void resetDrawnCards();
	std::map<EnumCharacter, std::shared_ptr<Character>> getCharactersDeck() const { return charactersDeck_; }
	std::pair<EnumCharacter, std::shared_ptr<Character>> removeCharacter(EnumCharacter character);
	bool moveCharacterFromDecktoPlayer(EnumCharacter character, std::shared_ptr<Player>);

	void putBackToBuildingsDeck(std::shared_ptr<Card> card);

	void murderCharacter(EnumCharacter character);
	void markForTheft(EnumCharacter character);
	EnumCharacter getMurderTarget();

private:
	void createBuildingCards();
	void createCharacterCards();
	std::vector<std::shared_ptr<Card>> buildingsDeck_;
	std::vector<std::shared_ptr<Card>> drawnCards_;
	std::map<EnumCharacter, std::shared_ptr<Character>> charactersDeck_;
	std::vector<std::shared_ptr<Player>> players_;
	std::vector<std::string> split(std::string& s, char delim);
	EnumState currentState_;
	bool usingAbility_ = false;
	bool goldStolen_ = false;
};

