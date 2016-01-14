#include "Card.h"
#include "EnumColor.h"

Card::Card(int id, std::string name, int value, EnumColor color)
{
	id_ = id;
	name_ = name;
	value_ = value;
	color_ = color;
}


Card::~Card()
{
}
