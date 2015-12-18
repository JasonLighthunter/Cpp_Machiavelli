#include "CommandHandler.h"
#include "ClientCommand.h"
#include "Socket.h"

using namespace std;
namespace machiavelli {
	const int tcp_port{1080};
	const string prompt{"machiavelli> "};
}

CommandHandler CommandHandler::instance;
//public
void CommandHandler::init(shared_ptr<Player> player, shared_ptr<Socket> socket) {
	if(playerCount_ < 2) {
		sockets_.push_back(socket);
		players_.push_back(player);
		playerCount_++;
	}
}

void CommandHandler::handleCommand(ClientCommand clientCmd) const {
	string cmd{clientCmd.get_cmd()};
	for(char &c : cmd) {
		c = tolower(c);
	}
//	gebruik hier
//	-	writeMessageToActivePlayer(string message);
//	-	writeMessageToAll
}

//private
void CommandHandler::writeMessageToActivePlayer(string message) {
	getActiveSocket()->write(prepareMessage(message));
}

void CommandHandler::writeMessageToAll(string message) {
	for(shared_ptr<Socket> socket:sockets_) {
		socket->write(prepareMessage(message));
	}
}

string CommandHandler::prepareMessage(string messages) {
	return messages+"\r\n"+machiavelli::prompt;
}
