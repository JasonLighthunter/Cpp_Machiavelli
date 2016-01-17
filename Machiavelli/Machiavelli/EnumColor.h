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

const std::map<EnumColor, std::string> convertEnumColorToString = {
	{ EnumColor::YELLOW, "geel" },
	{ EnumColor::BLUE, "blauw" },
	{ EnumColor::GREEN, "groen" },
	{ EnumColor::RED, "rood" }
};