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
	std::shared_ptr<Socket> getActiveSocket() const { return sockets_[turnCounter_%2]; }
	void init(std::shared_ptr<Player>, std::shared_ptr<Socket>);
private:
	//variables
	static CommandHandler instance;
	CommandHandler() {};
	std::vector<std::shared_ptr<Socket>> sockets_;
	std::unique_ptr<Game> game_;
	int turnCounter_ = 0;
	int playerCount_ = 0;

	//methods
	void writeMessageToActivePlayer(ClientCommand clientCmd, std::string message);
	void writeReply(ClientCommand clientCmd, std::string message);
	void writeMessageToAll(std::string message);
	std::string prepareMessage(std::string messages);

	void showPossibleCharacters(ClientCommand clientCmd);
	void showTurnInfo(ClientCommand clientCmd);
	bool requestingPlayerHasRightRole(ClientCommand clientCmd);

	void handleNormalStartCommand(ClientCommand clientCmd);
	void handleStartCommands(ClientCommand clientCmd, std::string cmd);
	void handleQuickStartCommand(ClientCommand clientCmd);

	void handleSetupCommand(EnumCharacter character, ClientCommand clientCmd);

	void handlePassCommand(ClientCommand clientCmd);
	void handleEndOfGame(ClientCommand clientCmd);
};
#endif
