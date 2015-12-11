#ifndef Player_hpp
#define Player_hpp

#include <string>

class Player {
public:
	Player() {}
	Player(const std::string& name) : name{name} {}

	std::string get_name() const { return name; }
	void set_name(const std::string& new_name) { name = new_name; }

private:
	std::string name;
};

#endif /* Player_hpp */
