#pragma once

#include <map>

enum class EnumColor {
	GEEL,
	BLAUW,
	GROEN,
	ROOD
};

const std::map<std::string, EnumColor> convertToEnumColor = {
	{ "geel", EnumColor::GEEL },
	{ "blauw", EnumColor::BLAUW },
	{ "groen", EnumColor::GROEN },
	{ "rood", EnumColor::ROOD }
};