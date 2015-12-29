#include "Card.h"
#include "EnumColor.h"

Card::Card(std::string name, int value, EnumColor color)
{
	name_ = name;
	value_ = value;
	color_ = color;
}


Card::~Card()
{
}
