#pragma once

#include <string>

enum class EnumColor;

class Card
{
public:
	Card(int id, std::string name, int value, EnumColor color);
	~Card();
	int getCosts() const { return value_; }
	int getId() const { return value_; }
private:
	int id_;
	std::string name_;
	int value_;
	EnumColor color_;
};

