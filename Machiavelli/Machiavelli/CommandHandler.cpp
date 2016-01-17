#include <random>
#include <iostream>
#include "CommandHandler.h"
#include "ClientCommand.h"
#include "Socket.h"
#include "EnumState.h"
#include "EnumCharacter.h"
#include "EnumTurnState.h"
#include "Player.hpp"
#include "Character.h"
#include "Card.h"

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
	if(cmd == "start" || cmd == "begin" || cmd == "quickstart") {
		if(game_->getCurrentState() == EnumState::UNSTARTED && playerCount_ == 2) {
			handleStartCommands(clientCmd, cmd);
		} else {
			writeReply(clientCmd, "Het is nu niet mogelijk om een spel te starten.");
		}
	} else if(cmd=="moordenaar"||cmd=="dief"||cmd=="magier"||cmd=="koning"||cmd=="prediker"||cmd=="koopman"||cmd=="bouwmeester"||cmd=="condottiere") {
		if((game_->getCurrentState()==EnumState::SETUP_CHOOSE||
			game_->getCurrentState()==EnumState::SETUP_DISCARD||
			game_->getCurrentState()==EnumState::SETUP_CHOOSE_FIRST)&&
		   clientCmd.getPlayer()==game_->getPlayerOnIndex(turnCounter_)) {
			handleSetupCommands(convertToEnumCharacter.at(cmd), clientCmd);
		} else if(requestingPlayerHasRightRole(clientCmd)&&game_->usingAbility()) {
			handleAbilityCommand(cmd, clientCmd);
		} else {
			writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
		}
	} else if(cmd=="goud") {
		handleGetGoldCommand(clientCmd);
	} else if(cmd == "gebouwen"){
		handleGetBuildingCommand(clientCmd);
	} else if (cmd == "bouw") {
		handleBuildBuildingCommand(clientCmd);
	} else if (cmd == "eigenschap") {
		handleStartAbilityCommand(clientCmd);
	} else if (cmd == "terug"){
		handleBackCommand(clientCmd);
	} else if(cmd == "einde beurt" || cmd == "pas" || cmd == "eind"){
		handlePassCommand(clientCmd);
	} else if (clientCmd.getPlayer()->getCurrentTurnState() == EnumTurnState::CHOOSE_BUILDING) {
		handleChooseBuildingCommand(clientCmd);
	} else if (clientCmd.getPlayer()->getCurrentTurnState() == EnumTurnState::BUILD_BUILDING) {
		handleChooseToBuildCommand(clientCmd);
	} else {
		writeReply(clientCmd, "Onbekend commando ontvangen.");
	}
}

//private
void CommandHandler::writeMessageToActivePlayer(ClientCommand clientCmd, string message) {
	int index = -1;

	switch(game_->getCurrentState()) {
		case EnumState::UNSTARTED:
		case EnumState::SETUP_CHOOSE_FIRST:
		case EnumState::SETUP_CHOOSE:
		case EnumState::SETUP_DISCARD:
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
	boolean done = false;

	game_->switchState(EnumState::SETUP_CHOOSE_FIRST);
	game_->init();

	string name = clientCmd.getPlayer()->getName();
	if(!(game_->getPlayerName(turnCounter_) == name)) {
		turnCounter_++;
	}
	writeMessageToAll("Het spel begint nu.\n\r" + name + " is de eerste ronde de koning!");
	while(!done) {
		random_device dev;
		default_random_engine dre{dev()};
		uniform_int_distribution<int> dist{0, static_cast<int>(convertFromEnumCharacter.size())-1};
		int chosenNumber=dist(dre);
		try {
			if(!(static_cast<EnumCharacter>(chosenNumber) == EnumCharacter::KING)) {
				game_->removeCharacter(static_cast<EnumCharacter>(chosenNumber));
				done = true;
			}
		} catch(...) {
			cerr << "characterDeck_ does not contain: " << convertFromEnumCharacter.at(static_cast<EnumCharacter>(chosenNumber)) << "\n";
		}
	}
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
	game_->initQuickStart();
	writeMessageToAll("");
	showTurnInfo(clientCmd);
}

void CommandHandler::handleSetupCommands(EnumCharacter character, ClientCommand clientCmd) {
	switch(game_->getCurrentState()) {
		case EnumState::SETUP_CHOOSE_FIRST:
			handleChooseCharacterFirstCommand(character, clientCmd);
			break;
		case EnumState::SETUP_CHOOSE:
			handleChooseCharacterCommand(character, clientCmd);
			break;
		case EnumState::SETUP_DISCARD:
			handleDiscardCharacterCommand(character, clientCmd);
			break;
		default:
			break;
	}
}
void CommandHandler::handleChooseCharacterFirstCommand(EnumCharacter character, ClientCommand clientCmd) {
	writeMessageToActivePlayer(clientCmd, "Jij koos: "+convertFromEnumCharacter.at(character));

	if(!game_->moveCharacterFromDecktoPlayer(character, clientCmd.getPlayer())) {
		writeMessageToActivePlayer(clientCmd, "...maar die is niet beschikbaar. Probeer het opnieuw.");
	} else {
		turnCounter_++;
		game_->switchState(EnumState::SETUP_CHOOSE);
		showPossibleCharacters(clientCmd);
	}
}
void CommandHandler::handleChooseCharacterCommand(EnumCharacter character, ClientCommand clientCmd) {
	writeMessageToActivePlayer(clientCmd, "Jij koos: " + convertFromEnumCharacter.at(character));

	if(!game_->moveCharacterFromDecktoPlayer(character, clientCmd.getPlayer())) {
		writeMessageToActivePlayer(clientCmd, "...maar die is niet beschikbaar. Probeer het opnieuw.");
	} else {
		game_->switchState(EnumState::SETUP_DISCARD);
		showPossibleCharacters(clientCmd);
	}
}
void CommandHandler::handleDiscardCharacterCommand(EnumCharacter character, ClientCommand clientCmd) {
	writeMessageToActivePlayer(clientCmd, "Jij koos: " + convertFromEnumCharacter.at(character));

	try {
		game_->removeCharacter(character);
		game_->switchState(EnumState::SETUP_CHOOSE);
		turnCounter_++;
		showPossibleCharacters(clientCmd);
	} catch(...) {
		writeMessageToActivePlayer(clientCmd, "...maar die is niet beschikbaar. Probeer het opnieuw.");
	}
}

void CommandHandler::handleBackCommand(ClientCommand clientCmd) {
	if(!(requestingPlayerHasRightRole(clientCmd))) {
		writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
	} else if(game_->usingAbility()) {
		game_->setUsingAbility(false);
		showTurnInfo(clientCmd);
	} else {
		writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
	}
}

void CommandHandler::handleStartAbilityCommand(ClientCommand clientCmd) {
	game_->setUsingAbility(true);
	if(game_->abilityUsed(stateToCharacter.at(game_->getCurrentState()))) {
		if(requestingPlayerHasRightRole(clientCmd)) {
			writeMessageToActivePlayer(clientCmd, "Je hebt je eigenschap al gebruikt.");
		}
		handleBackCommand(clientCmd);
	} else {
		switch(game_->getCurrentState()) {
			case EnumState::ASSASSIN_STATE:
				writeMessageToActivePlayer(clientCmd, "Welke rol wil je vermoorden?\r\n [dief, magier, koning, prediker, koopman, bouwmeester, condottiere, terug]");
				break;
			case EnumState::THIEF_STATE:
				writeMessageToActivePlayer(clientCmd, "Welke rol wil je bestelen?\r\n [terug]");
				break;
			case EnumState::MAGICIAN_STATE:
				writeMessageToActivePlayer(clientCmd, "Wat wil je doen?\r\n [terug]");
				break;
			case EnumState::KING_STATE:
			case EnumState::BISHOP_STATE:
			case EnumState::ARCHITECT_STATE:
			case EnumState::MERCHANT_STATE:
				writeMessageToActivePlayer(clientCmd, "De "+convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState()))+" heeft geen speciale eigenschappen (Die hij op deze manier kan gebruiken).\r\n [terug]");
				break;
			case EnumState::WARLORD_STATE:
				writeMessageToActivePlayer(clientCmd, "Wiens gebouwen wil je vernietigen?\r\n [terug]");
				break;
			default:
				break;
		}
	}
}
void CommandHandler::handleAbilityCommand(string cmd, ClientCommand clientCmd) {
	switch(game_->getCurrentState()) {
		case EnumState::ASSASSIN_STATE:
			handleMurderAbilityCommand(cmd, clientCmd);
			break;
		case EnumState::THIEF_STATE:
			//TODO implement
			break;
		case EnumState::MAGICIAN_STATE:
			//TODO implement
			break;
		case EnumState::KING_STATE:
		case EnumState::BISHOP_STATE:
		case EnumState::ARCHITECT_STATE:
		case EnumState::MERCHANT_STATE:
			writeReply(clientCmd, "Je kunt dit commando nu niet gebruiken");
			break;
		case EnumState::WARLORD_STATE:
			//TODO implement
		default:
			break;
	}
	handleBackCommand(clientCmd);
}

void CommandHandler::handleMurderAbilityCommand(string cmd, ClientCommand clientCmd) {
	if(cmd != "moordenaar") {
		game_->murderCharacter(convertToEnumCharacter.at(cmd));
		game_->setAbilityUsed(true, EnumCharacter::ASSASSIN);
		writeMessageToAll("De " + cmd + " is vermoord door de moordenaar.");
	} else {
		writeMessageToActivePlayer(clientCmd, "Je kunt jezelf niet vermoorden.");
	}
}

void CommandHandler::handlePassCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);
	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	} else {
		game_->switchState(nextState.at(game_->getCurrentState()));
		showTurnInfo(clientCmd);
	}
}

void CommandHandler::handleGetGoldCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);

	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	} else {
		auto player = clientCmd.getPlayer();
		auto character = player->getCharacter(stateToCharacter.at(game_->getCurrentState()));
		player->increaseGold(2);
		character->setGoldOrBuilding(true);
		writeReply(clientCmd, "Je goud is opgehoogd naar " + to_string(player->getGold()));
		showTurnInfo(clientCmd);
	}
}

void CommandHandler::handleGetBuildingCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);

	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	} else {
		auto player = clientCmd.getPlayer();
		player->setCurrentTurnState(EnumTurnState::CHOOSE_BUILDING);
		string message = "\n\r\r\nWelk gebouw wil je in je hand nemen:\r\n";
		game_->drawCards(2);
		for (auto card : game_->getDrawnCards()) {
			message += "-   [" + card->getName() + "](" + to_string(card->getCosts()) + ")\r\n";
		}
		message += "[annuleer]\r\n";

		writeMessageToActivePlayer(clientCmd, message);
	}
}

void CommandHandler::handleChooseBuildingCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);

	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	} else {
		auto player = clientCmd.getPlayer();
		if (clientCmd.getCmd() == "annuleer") {
			player->setCurrentTurnState(EnumTurnState::DEFAULT);
			showTurnInfo(clientCmd);
		} else {
			bool success = false;
			int i = 0;
			for (auto card : game_->getDrawnCards()) {
				if (card->getName() == clientCmd.getCmd()) {
					player->addBuildingCard(card);
					game_->getDrawnCards().erase(game_->getDrawnCards().begin() + i);
					game_->resetDrawnCards();
					player->setCurrentTurnState(EnumTurnState::DEFAULT);
					auto character = player->getCharacter(stateToCharacter.at(game_->getCurrentState()));
					character->setGoldOrBuilding(true);
					success = true;
					break;
				}
				i++;
			}
			if (success) {
				showTurnInfo(clientCmd);
			} else {
				writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken\n");
			}
		}
	}
}

void CommandHandler::handleBuildBuildingCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);

	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	} else {
		auto player = clientCmd.getPlayer();
		if (player->getHand().size() > 0) {
			auto character = player->getCharacter(stateToCharacter.at(game_->getCurrentState()));
			player->setCurrentTurnState(EnumTurnState::BUILD_BUILDING);
			string message = "\n\r\r\nWelk gebouw wil je bouwen:\r\n";

			for (auto building : player->getHand()) {
				message += "-   [" + building.second->getName() + "](" + to_string(building.second->getCosts()) + ")\r\n";
			}
			message += "[annuleer]\r\n";
			writeMessageToActivePlayer(clientCmd, message);
		} else {
			writeReply(clientCmd, "Je hebt geen gebouwen in je hand");
		}
	}
}


void CommandHandler::handleChooseToBuildCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);

	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	}
	else {
		auto player = clientCmd.getPlayer();
		auto character = player->getCharacter(stateToCharacter.at(game_->getCurrentState()));
		if (clientCmd.getCmd() == "annuleer" || character->buildedBuilding()) {
			if (character->buildedBuilding()) {
				writeMessageToActivePlayer(clientCmd, "Je hebt al een gebouw gebouwd.");
			}
			player->setCurrentTurnState(EnumTurnState::DEFAULT);
			showTurnInfo(clientCmd);
		}
		else {
			bool success = player->buildBuilding(clientCmd.getCmd());
			if (success) {
				character->setBuildedBuilding(true);
				showTurnInfo(clientCmd);
			}
			else {
				writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken\n");
			}
		}
	}
}
void CommandHandler::handleEndOfRound(ClientCommand clientCmd) {
	writeMessageToAll("Bedankt voor het meespelen. Nu start de volgende ronde.");
	int i = game_->getIndexOfKing();
	bool done = false;

	if(i == -1) {
		turnCounter_-= 3;
	} else {
		turnCounter_= i;
	}
	game_->resetGameToSetup();
	while(!done) {
		random_device dev;
		default_random_engine dre{dev()};
		uniform_int_distribution<int> dist{0, static_cast<int>(convertFromEnumCharacter.size()) - 1};
		int chosenNumber = dist(dre);
		try {
			if(!(static_cast<EnumCharacter>(chosenNumber) == EnumCharacter::KING)) {
				game_->removeCharacter(static_cast<EnumCharacter>(chosenNumber));
				done = true;
			}
		} catch(...) {
			cerr << "characterDeck_ does not contain: " << convertFromEnumCharacter.at(static_cast<EnumCharacter>(chosenNumber)) << "\n";
		}
	}
	showPossibleCharacters(clientCmd);
}

void CommandHandler::showTurnInfo(ClientCommand clientCmd) {
	if(!(game_->getCurrentState() == EnumState::END)) {
		string message = "\n\r\r\nHet is nu de beurt van de " +
			convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState()));
		writeMessageToAll(message);
		message = "\r\r\nJou rollen:\r\n";

		for(shared_ptr<Player> player : game_->getPlayers()) {
			string messagePlus = message;
			if(player->hasRole(stateToCharacter.at(game_->getCurrentState()))) {
				for(pair<EnumCharacter, shared_ptr<Character>> pair:player->getRoles()) {
					messagePlus+="-   " + convertFromEnumCharacter.at(pair.first) + "\r\n";
				}
				messagePlus+= "\r\r\nHoeveelheid goud: " + to_string(player->getGold()) + "\r\r\r\n\nJouw hand:\r\n";
				for (auto card : player->getHand()) {
					messagePlus += "-   " + card.second->getName() + "(" + to_string(card.second->getCosts()) + ")\r\n";
				}
				messagePlus+= "\r\r\nJouw gebouwen:\r\n";
				for (auto card : player->getBuildings()) {
					messagePlus+= "-   " + card.second->getName() + "(" + to_string(card.second->getCosts()) + ")\r\n";
				}
				messagePlus+= "\r\nWat wil je doen\r\n Opties: [pas";
				if(!game_->abilityUsed(stateToCharacter.at(game_->getCurrentState()))) {
					messagePlus+= ", eigenschap";
				}
				if(!player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->alreadyGetGoldOrBuilding()) {
					messagePlus+= ", goud, gebouwen";
				}
				if (!player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->buildedBuilding()) {
					messagePlus += ", bouw";
				}
				writeMessageToActivePlayer(clientCmd, messagePlus + "]");
				return;
			}
		}

		writeMessageToAll("\r\nDe beurt was aan de " + convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState())) + ", maar die is niet in het spel!");
		game_->switchState(nextState.at(game_->getCurrentState()));
		showTurnInfo(clientCmd);
	} else {
		handleEndOfRound(clientCmd);
	}
}

void CommandHandler::showPossibleCharacters(ClientCommand clientCmd) {
	string resultLine = "\n\n\r";

	if(game_->getCharactersDeck().size() > 0) {
		for(pair<EnumCharacter, shared_ptr<Character>> character : game_->getCharactersDeck()) {
			resultLine+= convertFromEnumCharacter.at(character.first) + "\r\n";
		}
		switch(game_->getCurrentState()) {
			case EnumState::SETUP_CHOOSE_FIRST:
			case EnumState::SETUP_CHOOSE:
				resultLine+= "\n\rKies de rol die je wilt hebben door de naam in te geven.";
				break;
			case EnumState::SETUP_DISCARD:
				resultLine+= "\n\rKies de rol die je wilt afleggen door de naam in te geven.";
				break;
			default:
				cerr << "This Shoud not happen. Non setup state encountered during setup;";
				break;
		}
		writeMessageToActivePlayer(clientCmd, resultLine);
	} else {
		cout << "CharacterDeck_ is out of cards start game.\n\r";
		game_->switchState(EnumState::ASSASSIN_STATE);
		showTurnInfo(clientCmd);
	}
}

bool CommandHandler::requestingPlayerHasRightRole(ClientCommand clientCmd) {
	try {
		return clientCmd.getPlayer()->hasRole(stateToCharacter.at(game_->getCurrentState()));
	} catch(...) {
		return false;
	}
}

bool CommandHandler::canUseCommand(ClientCommand clientCmd) {
	bool hasRightRole = requestingPlayerHasRightRole(clientCmd);

	switch (game_->getCurrentState()) {
	case EnumState::UNSTARTED:
	case EnumState::SETUP_CHOOSE:
	case EnumState::SETUP_CHOOSE_FIRST:
	case EnumState::SETUP_DISCARD:
	case EnumState::END:
		return false;
	case EnumState::ASSASSIN_STATE:
	case EnumState::THIEF_STATE:
	case EnumState::MAGICIAN_STATE:
	case EnumState::KING_STATE:
	case EnumState::BISHOP_STATE:
	case EnumState::ARCHITECT_STATE:
	case EnumState::MERCHANT_STATE:
	case EnumState::WARLORD_STATE:
		if (!hasRightRole) {
			return false;
		}
		else {
			if (clientCmd.getCmd() == "goud" || clientCmd.getCmd() == "gebouwen") {
				auto player = clientCmd.getPlayer();
				auto character = player->getCharacter(stateToCharacter.at(game_->getCurrentState()));
				if (!character->alreadyGetGoldOrBuilding()) {
					return true;
				}
				return false;
			}
			return true;
		}
	default:
		return false;
	}
}
