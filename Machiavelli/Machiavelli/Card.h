#pragma once

#include <string>

enum class EnumColor;

class Card
{
public:
	Card(std::string name, int value, EnumColor color);
	~Card();
private:
	std::string name_;
	int value_;
	EnumColor color_;
};

