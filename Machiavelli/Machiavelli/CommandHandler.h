#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <utility>
#include <memory>
#include <vector>
#include "Game.h"

class Socket;
class ClientCommand;
class Player;

class CommandHandler {
public:
	static CommandHandler& getInstance() { return instance; }
	void handleCommand(ClientCommand clientCmd);
	std::shared_ptr<Socket> getActiveSocket() const { return sockets_[turnCounter_ % 2]; }
	void init(std::shared_ptr<Player>, std::shared_ptr<Socket>);
private:
	//variables
	static CommandHandler instance;
	CommandHandler() {};
	std::vector<std::shared_ptr<Socket>> sockets_;
	std::unique_ptr<Game> game_;
	int turnCounter_ = 0;
	int playerCount_ = 0;
	bool letKnownWhichCharacterPlays_ = false;

	//methods
	void writeMessageToActivePlayer(ClientCommand clientCmd, std::string message); //schrijft bericht naar huidige speler; aka de moordenaar in de beurt van de moordenaar;
	void writeReply(ClientCommand clientCmd, std::string message); //schrijft een bericht naar degene wiens commando wordt afgehandelt.
	void writeMessageToAll(std::string message); //schrijft bericht naar iedereen.
	std::string prepareMessage(std::string messages);

	void showPossibleCharacters(ClientCommand clientCmd);
	void showTurnInfo(ClientCommand clientCmd); // toont mogelijke acties voor een beurt.
	bool requestingPlayerHasRightRole(ClientCommand clientCmd); //geeft aan of een speler de juiste rol heeft;

	void handleNormalStartCommand(ClientCommand clientCmd); //begint gewone setup
	void handleStartCommands(ClientCommand clientCmd, std::string cmd); //kiest of gewone of quickstartup wordt gekozen;
	void handleQuickStartCommand(ClientCommand clientCmd); // dief koning voor een speler moordenaar koopman voor de ander;

	void handleSetupCommands(EnumCharacter character, ClientCommand clientCmd); // kiest of discard of choose variant van de setup commandos moet worden uitgevoerd;
	void handleChooseCharacterFirstCommand(EnumCharacter character, ClientCommand clientCmd); //deze werkt iets anders in verband met de eerste keer karakter kiezen.
	void handleChooseCharacterCommand(EnumCharacter character, ClientCommand clientCmd); //laat speler karakter kiezen die hij/zij wil hebben.
	void handleDiscardCharacterCommand(EnumCharacter character, ClientCommand clientCmd); // laat speler karkater kiezen die niet meedoet deze ronde;

	void handleBackCommand(ClientCommand clientCmd); //gaat terug naar het initiele beurt overzicht aka [pas eigenschap inkomstten etc]
	void handleStartAbilityCommand(ClientCommand clientcmd); //toggled de boolean usingability die ervoorzorgt dat er een promt verschijnt met de keuzes voor de speciale vaardigheid.
	void handleAbilityCommand(std::string cmd, ClientCommand clientcmd);

	void handleMurderAbilityCommand(std::string cmd, ClientCommand clientCmd); //vermoordt een karakter en zorgt ervoor dat deze stilletjes wordt overgeslagen deze ronde.
	void handleTheftAbilityCommand(std::string cmd, ClientCommand clientCmd); //toggled de boolean isTheftTarget. die ervoor zorgt dat al het goud aan het begin van de beurt van de gekozen rol naar de dief gaat.

	void handleBishopAbilityCommand(ClientCommand clientCmd);

	void handleMerchantAbilityCommand(ClientCommand clientCmd);

	void handleWarlordAbilityCommand(ClientCommand clientCmd);

	void handleBeginDestroyBuildingCommand(ClientCommand clientCmd);

	void handleDestroyBuildingAbilityCommand(std::string cmd, ClientCommand clientCmd);

	void handlePassCommand(ClientCommand clientCmd);
	void handleChooseToBuildCommand(ClientCommand clientCmd);
	//beeindigt de beurt.
	void handleEndOfRound(ClientCommand clientCmd); //TODO checken op win conditites. start nieuew ronde.

	void handleGetGoldCommand(ClientCommand clientCmd);

	void handleGetBuildingCommand(ClientCommand clientCmd);
	void handleChooseBuildingCommand(ClientCommand clientCmd);
	void handleBuildBuildingCommand(ClientCommand clientCmd);
	bool canUseCommand(ClientCommand clientCmd);
};
#endif
