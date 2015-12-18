//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <string>

class Player {
public:
	Player() {}
	Player(const std::string& name) : name {name} {}
	
	std::string getName() const { return name; }
	void setName(const std::string& new_name) { name = new_name; }
private:
	std::string name;
};

#endif /* Player_hpp */
