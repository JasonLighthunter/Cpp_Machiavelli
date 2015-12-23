//
//  main.cpp
//  socketexample
//
//  Created by Bob Polis on 16/09/14.
//  Copyright (c) 2014 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include "Socket.h"
#include "Sync_queue.h"
#include "ClientCommand.h"
#include "Player.hpp"
#include <iostream>
#include <vector>
#include "CommandHandler.h"

using namespace std;

namespace machiavelli {
	const int tcp_port {1080};
	const string prompt {"machiavelli> "};
}

static Sync_queue<ClientCommand> queue;

// runs in its own thread
void consume_command() {
	try {
		while (true) {
			ClientCommand command {queue.get()}; // will block here unless there are still command objects in the queue
			shared_ptr<Socket> client {command.get_client()};
			shared_ptr<Player> player {command.get_player()};
			try {
				//CustomCode
				CommandHandler::getInstance().handleCommand(command);
			} catch (const exception& ex) {
				cerr << "*** exception in consumer thread for player " << player->getName() << ": " << ex.what() << '\n';
				if (client->is_open()) {
					client->write("Sorry, something went wrong during handling of your request.\r\n");
				}
			} catch (...) {
				cerr << "*** exception in consumer thread for player " << player->getName() << '\n';
				if (client->is_open()) {
					client->write("Sorry, something went wrong during handling of your request.\r\n");
				}
			}
		}
	} catch (...) {
		cerr << "consume_command crashed\n";
	}
}

// this function runs in a separate thread
void handle_client(shared_ptr<Socket> client) {
	try {
		client->write("Welcome to Server 1.0! To quit, type 'quit'.\r\n");
		client->write("What's your name?\r\n");
		client->write(machiavelli::prompt);
		string name {client->readline()};
		shared_ptr<Player> player {new Player {name}};
		*client << "Welcome, " << name << ", have fun playing our game!\r\n" << machiavelli::prompt;
		CommandHandler::getInstance().init(player, client);
		while (true) { // game loop
			try {
				// read first line of request
				string cmd {client->readline()};
				cerr << '[' << client->get_dotted_ip() << " (" << client->get_socket() << ") " << player->getName() << "] " << cmd << '\n';
				
				if (cmd == "quit") {
					client->write("Bye!\r\n");
					break;
				}
				
				ClientCommand command {cmd, client, player};
				queue.put(command);
			} catch (const exception& ex) {
				cerr << "*** exception in client handler thread for player " << player->getName() << ": " << ex.what() << '\n';
				if (client->is_open()) {
					*client << "ERROR: " << ex.what() << "\r\n";
				}
			} catch (...) {
				cerr << "*** exception in client handler thread for player " << player->getName() << '\n';
				if (client->is_open()) {
					client->write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
				}
			}
		}
		if(client->is_open()) {
			client->close();
		}
	} catch (...) {
		cerr << "handle_client crashed\n";
	}
}

int main(int argc, const char * argv[]) {
	// start command consumer thread
	thread consumer {consume_command};

	// keep client threads here, so we don't need to detach them
	vector<thread> handlers;

	// create a server socket
	ServerSocket server {machiavelli::tcp_port};
	
	while (true) {
		try {
			while (true) {
				// wait for connection from client; will create new socket
				cerr << "server listening" << '\n';
				unique_ptr<Socket> client {server.accept()};

				// communicate with client over new socket in separate thread
				thread handler {handle_client, move(client)};
				handlers.push_back(move(handler));
			}
		} catch (const exception& ex) {
			cerr << ex.what() << ", resuming..." << '\n';
		} catch (...) {
			cerr << "problems, problems, but: keep calm and carry on!\n";
		}
	}
	return 0;
}
