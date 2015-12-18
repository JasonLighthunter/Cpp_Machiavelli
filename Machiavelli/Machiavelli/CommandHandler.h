#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <memory>
#include <vector>

class ClientCommand;
class Player;
class Socket;

class CommandHandler {
public:
	static CommandHandler& getInstance() { return instance; }
	void handleCommand(ClientCommand clientCmd) const;
	std::shared_ptr<Player> getActivePlayer() const { return players_[turnCounter%2]; }
	std::shared_ptr<Socket> getActiveSocket() const { return sockets_[turnCounter%2]; }
	void init(std::shared_ptr<Player>, std::shared_ptr<Socket>);
private:
	//variables
	static CommandHandler instance;
	CommandHandler() {};
	std::vector<std::shared_ptr<Player>> players_;
	std::vector<std::shared_ptr<Socket>> sockets_;
	int turnCounter = 0;
	int playerCount_ = 0;

	//methods
	void writeMessageToActivePlayer(std::string message);
	void writeMessageToAll(std::string message);
	std::string prepareMessage(std::string messages);
};
#endif
