#include <random>
#include <iostream>
#include "CommandHandler.h"
#include "ClientCommand.h"
#include "Socket.h"
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
	for(char &c : cmd) {
		c = tolower(c);
	}
	//on this indentation level: handling of commands.
		//on this indentation level: handling of state/context.
	if(cmd=="start"||cmd=="begin"||cmd=="quickstart") {
		if(game_->getCurrentState()==EnumState::UNSTARTED && playerCount_==2) {
			handleStartCommands(clientCmd, cmd);
		} else {
			writeReply(clientCmd, "Het is nu niet mogelijk om een spel te starten.");
		}
	} else if(cmd == "moordenaar" || cmd == "dief" || cmd == "magier" || cmd == "koning" ||
			  cmd == "prediker"	  || cmd == "koopman" || cmd == "bouwmeester" || cmd == "condottiere") {
		if(game_->getCurrentState() == EnumState::SETUP && clientCmd.getPlayer() == game_->getPlayerOnIndex(turnCounter_)) {
			handleSetupCommand(convertToEnumCharacter.at(cmd), clientCmd);
		} else {
			writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
		}
	} else if(cmd == "einde beurt" || cmd == "pas" || cmd == "eind"){
		handlePassCommand(clientCmd);
	} else {
		writeReply(clientCmd, "Onbekend commando ontvangen.");
	}
}

//private
void CommandHandler::writeMessageToActivePlayer(ClientCommand clientCmd, string message) {
	int index = -1;

	switch(game_->getCurrentState()) {
		case EnumState::UNSTARTED:
		case EnumState::SETUP:
			getActiveSocket()->write(prepareMessage(message));
			break;
		case EnumState::END:
		case EnumState::ASSASSIN_STATE:
		case EnumState::THIEF_STATE:
		case EnumState::MAGICIAN_STATE:
		case EnumState::KING_STATE:
		case EnumState::BISHOP_STATE:
		case EnumState::ARCHITECT_STATE:
		case EnumState::MERCHANT_STATE:
		case EnumState::WARLORD_STATE:
			for(shared_ptr<Player> player : game_->getPlayers()) {
				if(player->hasRole(stateToCharacter.at(game_->getCurrentState()))) {
					index = game_->getIndexOfPlayer(player);
					break;
				}
			}
			if(index >= 0 || index < static_cast<int>(sockets_.size())) {
				sockets_[index]->write(prepareMessage(message));
			} else {
				cerr<<"Message written to non-existant player\r\n";
			}
		default:
			break;
	}
}
void CommandHandler::writeReply(ClientCommand clientCmd, string message) {
	clientCmd.getClient()->write(prepareMessage(message));
}
void CommandHandler::writeMessageToAll(string message) {
	for(shared_ptr<Socket> socket:sockets_) {
		socket->write(prepareMessage(message));
	}
}

string CommandHandler::prepareMessage(string messages) {
	return messages+"\r\n"+machiavelli::prompt;
}

void CommandHandler::handleNormalStartCommand(ClientCommand clientCmd){
	game_->switchState(EnumState::SETUP);
	string name = clientCmd.getPlayer()->getName();
	if(!(game_->getPlayerName(turnCounter_) == name)) {
		turnCounter_++;
	}
	writeMessageToAll("Het spel begint nu.\n\r" + name + " is de eerste ronde de koning!");
	showPossibleCharacters(clientCmd);
}
void CommandHandler::handleStartCommands(ClientCommand clientCmd, string cmd) {
	if(cmd == "quickstart") {
		handleQuickStartCommand(clientCmd);
	} else {
		handleNormalStartCommand(clientCmd);
	}
}
void CommandHandler::handleQuickStartCommand(ClientCommand clientCmd) {
	//players_[0] krijgt moordenaar en koning
	game_->moveCharacterFromDecktoPlayer(EnumCharacter::ASSASSIN, game_->getPlayers().at(0));
	game_->moveCharacterFromDecktoPlayer(EnumCharacter::KING,	  game_->getPlayers().at(0));
	//players_[1] krijgt dief en koopman
	game_->moveCharacterFromDecktoPlayer(EnumCharacter::MERCHANT, game_->getPlayers().at(1));
	game_->moveCharacterFromDecktoPlayer(EnumCharacter::THIEF,	  game_->getPlayers().at(1));
	//remove everything else
	game_->removeCharacter(EnumCharacter::MAGICIAN);
	game_->removeCharacter(EnumCharacter::BISHOP);
	game_->removeCharacter(EnumCharacter::ARCHITECT);
	game_->removeCharacter(EnumCharacter::WARLORD);
	game_->switchState(EnumState::ASSASSIN_STATE);
	writeMessageToAll("");
	showTurnInfo(clientCmd);
}

void CommandHandler::handleSetupCommand(EnumCharacter character, ClientCommand clientCmd) {
	writeMessageToActivePlayer(clientCmd, "Jij koos: " + convertFromEnumCharacter.at(character));
	if(!game_->moveCharacterFromDecktoPlayer(character, clientCmd.getPlayer())) {
		writeMessageToActivePlayer(clientCmd, "...maar die is niet beschikbaar. Probeer het opnieuw.");
	} else {
		turnCounter_++;
		showPossibleCharacters(clientCmd);
	}
}

void CommandHandler::handlePassCommand(ClientCommand clientCmd) {
	bool hasRightRole = requestingPlayerHasRightRole(clientCmd);

	switch(game_->getCurrentState()) {
		case EnumState::UNSTARTED:
		case EnumState::SETUP:
		case EnumState::END:
			writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
			break;
		case EnumState::ASSASSIN_STATE:
		case EnumState::THIEF_STATE:
		case EnumState::MAGICIAN_STATE:
		case EnumState::KING_STATE:
		case EnumState::BISHOP_STATE:
		case EnumState::ARCHITECT_STATE:
		case EnumState::MERCHANT_STATE:
			if(!hasRightRole) {
				writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
			} else {
				game_->switchState(nextState.at(game_->getCurrentState()));
				showTurnInfo(clientCmd);
			}
			break;
		case EnumState::WARLORD_STATE:
			game_->switchState(nextState.at(game_->getCurrentState()));
			break;
		default:
			break;
	}
}

void CommandHandler::handleEndOfGame(ClientCommand clientCmd) {
	//TODO: change to something better
	writeMessageToAll("Bedankt voor het meespelen. Nu start de volgende ronde.");
	int i=game_->getIndexOfKing();
	if(i==-1) {
		turnCounter_-3;
	} else {
		turnCounter_=i;
	}
	game_->resetGameToSetup();
	showPossibleCharacters(clientCmd);
}

void CommandHandler::showTurnInfo(ClientCommand clientCmd) {
	while(!(game_->getCurrentState()==EnumState::END)) {
		string message = "\r\r\nHet is nu de beurt van de "+
						 convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState()))+
						 "\r\r\nJou rollen:\r\n";

		for(shared_ptr<Player> player:game_->getPlayers()) {
			if(player->hasRole(stateToCharacter.at(game_->getCurrentState()))) {
				string messagePlus=message;
				for(pair<EnumCharacter, shared_ptr<Character>> pair:player->getRoles()) {
					messagePlus+="-   "+convertFromEnumCharacter.at(pair.first)+"\r\n";
				}
				writeMessageToActivePlayer(clientCmd, messagePlus+"\r\nWat wil je doen\r\n Opties: [pas]");
				return;
			}
		}

		writeMessageToAll("De beurt was aan de "+
						  convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState()))+
						  ", maar die is niet in het spel!");
		game_->switchState(nextState.at(game_->getCurrentState()));
	}
	handleEndOfGame(clientCmd);
}

void CommandHandler::showPossibleCharacters(ClientCommand clientCmd) {
	boolean done = false;
	string resultLine = "\n\n\r";

	if(game_->getCharactersDeck().size()>0) {
		while(!done) {
			random_device dev;
			default_random_engine dre{dev()};
			uniform_int_distribution<int> dist{0, static_cast<int>(convertFromEnumCharacter.size())-1};
			int chosenNumber=dist(dre);
			try {
				if(!(static_cast<EnumCharacter>(chosenNumber)==EnumCharacter::KING)) {
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
		writeMessageToActivePlayer(clientCmd, resultLine);
	} else {
		cout<<"CharacterDeck_ is out of cards start game.\n\r";
		game_->switchState(nextState.at(game_->getCurrentState()));
		showTurnInfo(clientCmd);
	}
}

bool CommandHandler::requestingPlayerHasRightRole(ClientCommand clientCmd) {
	try {
		EnumCharacter character = stateToCharacter.at(game_->getCurrentState());
		return clientCmd.getPlayer()->hasRole(character);
	} catch(...) {
		return false;
	}
}
