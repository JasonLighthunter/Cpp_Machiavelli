#include <random>
#include <iostream>
#include "CommandHandler.h"
#include "ClientCommand.h"
#include "Socket.h"
#include "Game.h"
#include "EnumState.h"
#include "EnumCharacter.h"
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
	if(cmd=="start"||cmd=="begin") {
		if(game_->getCurrentState()==EnumState::UNSTARTED && playerCount_==2) {
			handleStartCommand(clientCmd);
		} else {
			writeReply(clientCmd, "Het is nu niet mogelijk om een spel te starten.");
		}
	} else if(cmd=="moordenaar"	|| cmd=="dief"	 || cmd=="magier"	  || cmd=="koning" ||
			  cmd=="prediker"	|| cmd=="koopman"|| cmd=="bouwmeester"||  cmd=="condottiere") {
		if(game_->getCurrentState()==EnumState::SETUP && clientCmd.getPlayer()==game_->getPlayerOnIndex(turnCounter_)) {
			handleSetupCommand(convertToEnumCharacter.at(cmd),clientCmd);
		} else {
			writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
		}
	} else {
		writeReply(clientCmd, "Onbekend commando ontvangen.");
	}
}

//private
void CommandHandler::writeMessageToActivePlayer(string message) {
	getActiveSocket()->write(prepareMessage(message));
}
void CommandHandler::writeReply(ClientCommand clientCmd, string message) {
	clientCmd.getClient()->write(prepareMessage(message));
}
void CommandHandler::writeMessageToAll(string message) {
	for(shared_ptr<Socket> socket:sockets_) {
		socket->write(prepareMessage(message));
	}
}
void CommandHandler::clearWindowActivePlayer() {
	size_t i = 0;
	string result="";
	while(i<20) {
		result+="\n\r";
		i++;
	}
	writeMessageToActivePlayer(result);
}

string CommandHandler::prepareMessage(string messages) {
	return messages+"\r\n"+machiavelli::prompt;
}

void CommandHandler::handleStartCommand(ClientCommand clientCmd){
	game_->switchState(EnumState::SETUP);
	string name = clientCmd.getPlayer()->getName();
	if(!(game_->getPlayerName(turnCounter_)==name)) {
		turnCounter_++;
	}
	writeMessageToAll("Het spel begint nu.\n\r"+name+" is de eerste ronde de koning!");
	showPossibleCharacters();
}

void CommandHandler::handleSetupCommand(EnumCharacter character, ClientCommand clientCmd) {
	writeMessageToActivePlayer("Jij koos: "+convertFromEnumCharacter.at(character));
	if(!game_->moveCharacterFromDecktoPlayer(character, clientCmd.getPlayer())) {
		writeMessageToActivePlayer("...maar die is niet beschikbaar. Probeer het opnieuw.");
	} else {
		turnCounter_++;
		showPossibleCharacters();
	}
}

void CommandHandler::showPossibleCharacters() {
	boolean done = false;
	string resultLine = "\n\n\r";

	clearWindowActivePlayer();

	if(game_->getCharactersDeck().size()>0) {
		while(!done) {
			random_device dev;
			default_random_engine dre{dev()};
			uniform_int_distribution<int> dist{0, static_cast<int>(convertFromEnumCharacter.size())-1};
			int chosenNumber=dist(dre);
			try {
				if(!(static_cast<EnumCharacter>(chosenNumber)==EnumCharacter::KONING)) {
					game_->removeCharacter(static_cast<EnumCharacter>(chosenNumber));
					done=true;
				}
			} catch(...) {
				cerr<<"characterDeck_ does not contain: "<<convertFromEnumCharacter.at(static_cast<EnumCharacter>(chosenNumber))<<"\n";
			}
		}

		for(pair<EnumCharacter, shared_ptr<Character>> character:game_->getCharactersDeck()) {
			resultLine+=convertFromEnumCharacter.at(character.first)+"\r\n";
		}
		resultLine+="\n\rKies de rol die je wilt hebben door de naam in te geven.";
		writeMessageToActivePlayer(resultLine);
	} else {
		cout<<"CharacterDeck_ is out of cards start game.\n\r";
	}
}
