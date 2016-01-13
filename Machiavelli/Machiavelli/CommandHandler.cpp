#include "CommandHandler.h"
#include "ClientCommand.h"
#include "Socket.h"
#include "Game.h"
#include "EnumState.h"
#include "Player.hpp"

using namespace std;

namespace machiavelli {
	const int tcp_port{1080};
	const string prompt{"machiavelli> "};
}

CommandHandler CommandHandler::instance;

//public
void CommandHandler::init(shared_ptr<Player> player ,shared_ptr<Socket> socket) {
	if(game_ == nullptr) {
		game_=unique_ptr<Game>{new Game()};
	}
	if(playerCount_ < 2) {
		sockets_.push_back(socket);
		game_->addPlayer(player);
		playerCount_++;
	}
}

void CommandHandler::handleCommand(ClientCommand clientCmd){
//	use here
//	-	writeMessageToActivePlayer(string message);		To write a message to active client.
//	-	writeMessageToAll();							To write a message to all clients.
	string cmd{clientCmd.getCmd()};
	for(char &c:cmd) {
		c=tolower(c);
	}
	//on this indentation level: handling of commands.
		//on this indentation level: handling of state/context.
	if(cmd == "start" || cmd == "begin") {
		if(game_->getCurrentState()==EnumState::UNSTARTED && playerCount_ == 2) {
			game_->switchState(EnumState::SETUP);
			string name = clientCmd.getPlayer()->getName();
			writeMessageToAll("Het spel begint nu.\n" + name + " is de eerste ronde de koning!");
		} else {
			writeMessageToActivePlayer("Het is niet mogelijk om een spel te starten je bent de enige speler.");
		}
	}else {
		writeMessageToActivePlayer("Onbekend commando ontvangen.");
	}
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
