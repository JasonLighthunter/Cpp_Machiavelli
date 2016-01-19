#include <random>
#include <iostream>
#include "CommandHandler.h"
#include "ClientCommand.h"
#include "Socket.h"
#include "EnumState.h"
#include "EnumCharacter.h"
#include "EnumTurnState.h"
#include "EnumColor.h"
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
	if (game_->getCurrentState() != EnumState::FINISHED) {
		string cmd{ clientCmd.getCmd() };
		for (char &c : cmd) {
			c = tolower(c);
		}

		if (cmd == "start" || cmd == "begin" || cmd == "quickstart") {
			if (game_->getCurrentState() == EnumState::UNSTARTED && playerCount_ == 2) {
				handleStartCommands(clientCmd, cmd);
			} else {
				writeReply(clientCmd, "Het is nu niet mogelijk om een spel te starten.");
			}
		} else if (cmd == "moordenaar" || cmd == "dief" || cmd == "magier" || cmd == "koning" || cmd == "prediker" || cmd == "koopman" || cmd == "bouwmeester" || cmd == "condottiere") {
			if ((game_->getCurrentState() == EnumState::SETUP_CHOOSE ||
				game_->getCurrentState() == EnumState::SETUP_DISCARD ||
				game_->getCurrentState() == EnumState::SETUP_CHOOSE_FIRST) &&
				clientCmd.getPlayer() == game_->getPlayerOnIndex(turnCounter_)) {
				handleSetupCommands(convertToEnumCharacter.at(cmd), clientCmd);
			} else if (requestingPlayerHasRightRole(clientCmd) && game_->usingAbility()) {
				handleAbilityCommand(cmd, clientCmd);
			} else {
				writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken.");
			}
		} else if (cmd == "goud") {
			handleGetGoldCommand(clientCmd);
		} else if (cmd == "gebouwen") {
			handleGetBuildingCommand(clientCmd);
		} else if (cmd == "bouw") {
			handleBuildBuildingCommand(clientCmd);
		} else if (cmd == "eigenschap") {
			handleStartAbilityCommand(clientCmd);
		} else if (cmd == "terug") {
			handleBackCommand(clientCmd);
		} else if (cmd == "pas") {
			handlePassCommand(clientCmd);
		} else if (clientCmd.getPlayer()->getCurrentTurnState() == EnumTurnState::CHOOSE_BUILDING) {
			handleChooseBuildingCommand(clientCmd);
		} else if (clientCmd.getPlayer()->getCurrentTurnState() == EnumTurnState::BUILD_BUILDING) {
			handleChooseToBuildCommand(clientCmd);
		} else if (clientCmd.getPlayer()->getCurrentTurnState() == EnumTurnState::DESTROY_BUILDING) {
			handleDestroyBuildingAbilityCommand(cmd, clientCmd);
		} else {
			writeReply(clientCmd, "Onbekend commando ontvangen.");
		}
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
		clientCmd.getPlayer()->setCurrentTurnState(EnumTurnState::DEFAULT);
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
				writeMessageToActivePlayer(clientCmd, "Welke rol wil je bestelen?\r\n [magier, koning, prediker, koopman, bouwmeester, condottiere, terug]");
				break;
			case EnumState::MAGICIAN_STATE:
				writeMessageToActivePlayer(clientCmd, "Wat wil je doen?\r\n [terug]");
				break;
			case EnumState::BISHOP_STATE:
				handleBishopAbilityCommand(clientCmd);
				break;
			case EnumState::MERCHANT_STATE:
				handleMerchantAbilityCommand(clientCmd);
				break;
			case EnumState::KING_STATE:
				handleKingAbilityCommand(clientCmd);
				break;
			case EnumState::ARCHITECT_STATE:
				writeMessageToActivePlayer(clientCmd, "De "+convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState()))+" heeft geen speciale eigenschappen (Die hij op deze manier kan gebruiken).\r\n [terug]");
				game_->setUsingAbility(false);
				break;
			case EnumState::WARLORD_STATE:
				// Voor het goud voor elk rood gebouw
				handleWarlordAbilityCommand(clientCmd);

				//Voor het kiezen van een gebouw om te vernietigen
				handleBeginDestroyBuildingCommand(clientCmd);
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
			handleTheftAbilityCommand(cmd, clientCmd);
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
			handleDestroyBuildingAbilityCommand(cmd, clientCmd);
			break;
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
void CommandHandler::handleTheftAbilityCommand(string cmd, ClientCommand clientCmd) {
	if(cmd != "moordenaar" && cmd != "dief" && cmd != convertFromEnumCharacter.at(game_->getMurderTarget())) {
		game_->markForTheft(convertToEnumCharacter.at(cmd));
		game_->setAbilityUsed(true, EnumCharacter::THIEF);
		writeMessageToAll("De dief is van plan de de " + cmd + " te bestelen.");
	} else {
		writeMessageToActivePlayer(clientCmd, "Dit is niet mogelijk.");
	}
}

void CommandHandler::handleBishopAbilityCommand(ClientCommand clientCmd) {
	auto player = clientCmd.getPlayer();
	int nrBlueBuildings = 0;

	for (auto building : player->getBuildings()) {
		if (building.second->getColor() == EnumColor::BLUE) {
			nrBlueBuildings++;
		}
	}

	player->increaseGold(nrBlueBuildings);
	player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->setAbilityUsed(true);
	if (nrBlueBuildings > 0) {
		writeMessageToActivePlayer(clientCmd, "Je hebt " + to_string(nrBlueBuildings) + " blauwe gebouwen, je goud is opgehoogd naar " + to_string(player->getGold()));
	} else {
		writeMessageToActivePlayer(clientCmd, "Je hebt geen blauw gebouw, je hoeveelheid goud is gelijk gebleven.");
	}
	handleBackCommand(clientCmd);
}
void CommandHandler::handleKingAbilityCommand(ClientCommand clientCmd) {
	shared_ptr<Player> player = clientCmd.getPlayer();
	int nrYellowBuildings = 0;

	for(auto building:player->getBuildings()) {
		if(building.second->getColor()==EnumColor::YELLOW) {
			nrYellowBuildings++;
		}
	}

	player->increaseGold(nrYellowBuildings);
	player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->setAbilityUsed(true);
	if(nrYellowBuildings > 0) {
		writeMessageToActivePlayer(clientCmd, "Je hebt "+to_string(nrYellowBuildings)+" gele gebouwen, je goud is opgehoogd naar "+to_string(player->getGold()));
	} else {
		writeMessageToActivePlayer(clientCmd, "Je hebt geen geel gebouw, je hoeveelheid goud is gelijk gebleven.");
	}
	handleBackCommand(clientCmd);
}

void CommandHandler::handleMerchantAbilityCommand(ClientCommand clientCmd) {
	auto player = clientCmd.getPlayer();
	int nrGreenBuildings = 0;

	for (auto building : player->getBuildings()) {
		if (building.second->getColor() == EnumColor::GREEN) {
			nrGreenBuildings++;
		}
	}

	player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->setAbilityUsed(true);
	player->increaseGold(1);
	writeMessageToActivePlayer(clientCmd, "Je krijgt 1 goud omdat je koopman bent, je hebt nu " + to_string(player->getGold()) + " goud.");
	
	if (nrGreenBuildings > 0) {
		player->increaseGold(nrGreenBuildings);
		writeMessageToActivePlayer(clientCmd, "Je hebt " + to_string(nrGreenBuildings) + " groene gebouwen, je goud is opgehoogd naar " + to_string(player->getGold()));
	}
	else {
		writeMessageToActivePlayer(clientCmd, "Je hebt geen groen gebouw, je hoeveelheid goud is gelijk gebleven.");
	}
	handleBackCommand(clientCmd);
}

void CommandHandler::handleWarlordAbilityCommand(ClientCommand clientCmd) {
	auto player = clientCmd.getPlayer();
	int nrRedBuildings = 0;

	for (auto building : player->getBuildings()) {
		if (building.second->getColor() == EnumColor::RED) {
			nrRedBuildings++;
		}
	}

	player->increaseGold(nrRedBuildings);
	player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->setAbilityUsed(true);
	if (nrRedBuildings > 0) {
		writeMessageToActivePlayer(clientCmd, "Je hebt " + to_string(nrRedBuildings) + " rode gebouwen, je goud is opgehoogd naar " + to_string(player->getGold()));
	}
	else {
		writeMessageToActivePlayer(clientCmd, "Je hebt geen rood gebouw, je hoeveelheid goud is gelijk gebleven.\r\n");
	}
}

void CommandHandler::handleBeginDestroyBuildingCommand(ClientCommand clientCmd) {
	auto player = clientCmd.getPlayer();
	auto enemy = game_->getEnemy(player);
	if (enemy->hasRole(EnumCharacter::BISHOP)) {
		writeMessageToActivePlayer(clientCmd, "Je kunt geen gebouw van de prediker vernietingen. \r\n");
		game_->setUsingAbility(false);
	} else if (enemy->getBuildings().size() == 0) {
		writeMessageToActivePlayer(clientCmd, "Je tegenstander heeft geen gebouwen. \r\n");
		game_->setUsingAbility(false);
	} else if (enemy->getBuildings().size() >= 8) {
		writeMessageToActivePlayer(clientCmd, "Je tegenstander heeft al meer dan 8 gebouwen. \r\n");
		game_->setUsingAbility(false);
	} else {
		string message = "Welk gebouw wil je vernietigen?\r\n";
		for (auto card : enemy->getBuildings()) {
			message += "-   " + card.second->getName() + "(" + to_string(card.second->getCosts()) + ")(" + convertEnumColorToString.at(card.second->getColor()) + ")\r\n";
		}
		writeMessageToActivePlayer(clientCmd, message + "[terug]");
		player->setCurrentTurnState(EnumTurnState::DESTROY_BUILDING);
	}

}

void CommandHandler::handleDestroyBuildingAbilityCommand(string cmd, ClientCommand clientCmd) {
	auto player = clientCmd.getPlayer();
	auto enemy = game_->getEnemy(player);

	bool success = false;

	for (auto building : enemy->getBuildings()) {
		if (building.second->getName() == cmd) {
			int costs = building.second->getCosts() - 1;
			if (player->getGold() >= costs) {
				player->increaseGold(-costs);
				game_->putBackToBuildingsDeck(building.second);
				enemy->destroyBuilding(building);
				
				game_->setAbilityUsed(true, EnumCharacter::WARLORD);

				writeMessageToAll("De " + building.second->getName() + " van " + enemy->getName() + " is vernietigd door de condottiere.");
				writeMessageToActivePlayer(clientCmd, "Je goud is verlaagd naar " + to_string(player->getGold()) + ".");

				success = true;
			}
			break;
		}
	}

	if (!success) {
		writeMessageToActivePlayer(clientCmd, "Je hebt niet genoeg goud of je het een niet bestaand gebouw aangewezen.");
	}
	else {
		handleBackCommand(clientCmd);
	}
}

void CommandHandler::handlePassCommand(ClientCommand clientCmd) {
	bool canUse = canUseCommand(clientCmd);
	if (!canUse) {
		writeReply(clientCmd, "Je kan dit commando nu niet gebruiken.");
	} else {
		game_->switchState(nextState.at(game_->getCurrentState()));
		letKnownWhichCharacterPlays_ = false;
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
		if (stateToCharacter.at(game_->getCurrentState()) == EnumCharacter::ARCHITECT) {
			auto player = clientCmd.getPlayer();
			game_->drawCards(2);

			string message = "\n\r\r\nJe hebt de volgende kaarten in je hand genomen:\r\n";
			for (auto card : game_->getDrawnCards()) {
				message += "-   [" + card->getName() + "](" + to_string(card->getCosts()) + ")(" + convertEnumColorToString.at(card->getColor()) + ")\r\n";
			}

			writeMessageToActivePlayer(clientCmd, message);

			for (auto card : game_->getDrawnCards()) {
				player->addBuildingCard(card);
			}
			game_->getDrawnCards().clear();

			auto character = player->getCharacter(EnumCharacter::ARCHITECT);
			character->setGoldOrBuilding(true);

			showTurnInfo(clientCmd);
		}
		else {
			auto player = clientCmd.getPlayer();
			player->setCurrentTurnState(EnumTurnState::CHOOSE_BUILDING);
			string message = "\n\r\r\nWelk gebouw wil je in je hand nemen:\r\n";
			game_->drawCards(2);
			for (auto card : game_->getDrawnCards()) {
				message += "-   [" + card->getName() + "](" + to_string(card->getCosts()) + ")(" + convertEnumColorToString.at(card->getColor()) + ")\r\n";
			}
			message += "[annuleer]\r\n";

			writeMessageToActivePlayer(clientCmd, message);
		}
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
				message += "-   [" + building.second->getName() + "](" + to_string(building.second->getCosts()) + ")(" + convertEnumColorToString.at(building.second->getColor()) + ")\r\n";
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
		if (clientCmd.getCmd() == "annuleer" || (character->buildedBuildings() > 0 && character->getRole() != EnumCharacter::ARCHITECT)) {
			if (character->buildedBuildings() > 0) {
				writeMessageToActivePlayer(clientCmd, "Je hebt al een gebouw gebouwd.");
			}
			player->setCurrentTurnState(EnumTurnState::DEFAULT);
			showTurnInfo(clientCmd);
		}
		else {
			bool success = player->buildBuilding(clientCmd.getCmd());
			if (success) {
				if (player->getBuildings().size() >= 8 && !game_->getEnemy(player)->isFirstComplete()) {
					player->setFirstComplete();
				}
				character->increaseBuildedBuilding();
				showTurnInfo(clientCmd);
			}
			else {
				writeReply(clientCmd, "Je kunt dat commando nu niet gebruiken\n");
			}
		}
	}
}
void CommandHandler::handleEndOfRound(ClientCommand clientCmd) {
	bool finished = false;

	for (auto player : game_->getPlayers()) {
		if (player->isFirstComplete() || player->getBuildings().size() >= 8) {
			finished = true;
			break;
		}
	}

	if (!finished) {
		writeMessageToAll("Bedankt voor het meespelen. Nu start de volgende ronde.");
		int i = game_->getIndexOfKing();
		bool done = false;

		if (i == -1) {
			turnCounter_ -= 3;
		}
		else {
			turnCounter_ = i;
		}
		game_->resetGameToSetup();
		while (!done) {
			random_device dev;
			default_random_engine dre{ dev() };
			uniform_int_distribution<int> dist{ 0, static_cast<int>(convertFromEnumCharacter.size()) - 1 };
			int chosenNumber = dist(dre);
			try {
				if (!(static_cast<EnumCharacter>(chosenNumber) == EnumCharacter::KING)) {
					game_->removeCharacter(static_cast<EnumCharacter>(chosenNumber));
					done = true;
				}
			}
			catch (...) {
				cerr << "characterDeck_ does not contain: " << convertFromEnumCharacter.at(static_cast<EnumCharacter>(chosenNumber)) << "\n";
			}
		}
		showPossibleCharacters(clientCmd);
	} else {
		handleEndOfGame();
	}
}

void CommandHandler::handleEndOfGame() {
	int pointsPlayer1 = 0;
	int pointsPlayer2 = 0;

	auto players = game_->getPlayers();
	
	auto player1 = players[0];
	auto player2 = players[1];

	vector<EnumColor> color1;
	vector<EnumColor> color2;

	for (auto building : player1->getBuildings()) {
		pointsPlayer1 += building.second->getCosts();
		color1.push_back(building.second->getColor());
	}

	for (auto building : player2->getBuildings()) {
		pointsPlayer2 += building.second->getCosts();
		color2.push_back(building.second->getColor());
	}
	
	color1.erase(unique(color1.begin(), color1.end()), color1.end());
	color2.erase(unique(color2.begin(), color2.end()), color2.end());

	if (color1.size() >= 4) {
		pointsPlayer1 += 3;
	}
	if (color2.size() >= 4) {
		pointsPlayer2 += 3;
	}

	if (player1->isFirstComplete()) {
		pointsPlayer1 += 4;
	}
	else if (player1->getBuildings().size() >= 8) {
		pointsPlayer1 += 2;
	}

	if (player2->isFirstComplete()) {
		pointsPlayer2 += 4;
	}
	else if (player2->getBuildings().size() >= 8) {
		pointsPlayer2 += 2;
	}

	writeMessageToAll(player1->getName() + " heeft " + to_string(pointsPlayer1) + " punten!\r\n");
	writeMessageToAll(player2->getName() + " heeft " + to_string(pointsPlayer2) + " punten!\r\n");

	if (pointsPlayer1 > pointsPlayer2) {
		writeMessageToAll(player1->getName() + " heeft gewonnen!!!");
	}
	if (pointsPlayer1 < pointsPlayer2) {
		writeMessageToAll(player2->getName() + " heeft gewonnen!!!");
	}
	if (pointsPlayer1 == pointsPlayer2) {
		writeMessageToAll("Het spel is geëindigd met een gelijkspel!");
	}

	game_->switchState(EnumState::FINISHED);
}

void CommandHandler::showTurnInfo(ClientCommand clientCmd) {
	
	if(!(game_->getCurrentState() == EnumState::END)) {
		string message = "";
		if (!letKnownWhichCharacterPlays_) {
			message = "\n\r\r\nHet is nu de beurt van de " +
				convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState()));
			writeMessageToAll(message);
		}
		letKnownWhichCharacterPlays_ = true;
		message = "\r\r\nJou rollen:\r\n";

		for(shared_ptr<Player> player : game_->getPlayers()) {
			string messagePlus = message;
			if(player->hasRole(stateToCharacter.at(game_->getCurrentState()))) {

				for(pair<EnumCharacter, shared_ptr<Character>> pair:player->getRoles()) {
					messagePlus+="-   " + convertFromEnumCharacter.at(pair.first) + "\r\n";
				}
				//this is handled if role is marked for theft.
				if(player->isMarkedForTheft(stateToCharacter.at(game_->getCurrentState())) && !game_->goldStolen()) {
					//TODO: alter text
					cout << "The " << convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState())) << " was marked for Theft. Let the stealing commence!";
					int goldStolen=player->getGold();
					shared_ptr<Player> thief=game_->getPlayerWithRole(EnumCharacter::THIEF);
					player->decreaseGold(goldStolen);
					thief->increaseGold(goldStolen);
					game_->setGoldStolen(true);
				}

				messagePlus+= "\r\r\nHoeveelheid goud: " + to_string(player->getGold()) + "\r\r\r\n\nJouw hand:\r\n";
				for (auto card : player->getHand()) {
					messagePlus += "-   " + card.second->getName() + "(" + to_string(card.second->getCosts()) + ")("+ convertEnumColorToString.at(card.second->getColor()) +")\r\n";
				}
				messagePlus+= "\r\r\nJouw gebouwen:\r\n";
				for (auto card : player->getBuildings()) {
					messagePlus+= "-   " + card.second->getName() + "(" + to_string(card.second->getCosts()) + ")("+ convertEnumColorToString.at(card.second->getColor()) +")\r\n";
				}
				messagePlus+= "\r\nWat wil je doen\r\n Opties: [pas";
				if(!game_->abilityUsed(stateToCharacter.at(game_->getCurrentState())) && 
					stateToCharacter.at(game_->getCurrentState()) != EnumCharacter::ARCHITECT) {
					messagePlus+= ", eigenschap";
				}
				if(!player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->alreadyGetGoldOrBuilding()) {
					messagePlus+= ", goud, gebouwen";
				}
				if (player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->buildedBuildings() == 0 || 
					(stateToCharacter.at(game_->getCurrentState()) == EnumCharacter::ARCHITECT && player->getCharacter(stateToCharacter.at(game_->getCurrentState()))->buildedBuildings() < 3)) {
					messagePlus += ", bouw";
				}
				writeMessageToActivePlayer(clientCmd, messagePlus + "]");
				return;
			}
		}

		writeMessageToAll("\r\nDe beurt was aan de " + convertFromEnumCharacter.at(stateToCharacter.at(game_->getCurrentState())) + ", maar die is niet in het spel!");
		letKnownWhichCharacterPlays_ = false;
		game_->switchState(nextState.at(game_->getCurrentState()));
		game_->setGoldStolen(false);
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
