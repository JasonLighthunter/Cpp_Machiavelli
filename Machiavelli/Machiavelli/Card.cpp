#include "Card.h"
#include "EnumColor.h"

using namespace std;

Card::Card(int id, string name, int value, EnumColor color) {
	id_ = id;
	name_ = name;
	value_ = value;
	color_ = color;
}

Card::~Card() {}
