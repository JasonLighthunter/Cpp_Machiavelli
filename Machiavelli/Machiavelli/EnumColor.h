#pragma once

#include <map>

enum class EnumColor {
	YELLOW,
	BLUE,
	GREEN,
	RED
};

const std::map<std::string, EnumColor> convertToEnumColor = {
	{ "geel", EnumColor::YELLOW },
	{ "blauw", EnumColor::BLUE },
	{ "groen", EnumColor::GREEN },
	{ "rood", EnumColor::RED }
};