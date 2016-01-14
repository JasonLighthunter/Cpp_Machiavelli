#pragma once

#include "Range.h"

enum class EnumCard {
	TAVERN,
	MARKET,
	TRADING_POST,
	CASTLE,
	PALACE,
	STORES,
	CITY_HALL,
	HARBOUR,
	TEMPLE,
	CHURCH,
	CATHEDRAL,
	ESTATE,
	MONASTERY,
	WATCHTOWER,
	PRISON,
	BATTLEFIELD,
	FORTRESS
};

const std::map<std::string, EnumCard> convertToEnumCard = {
	{ "taveerne", EnumCard::TAVERN },
	{ "markt", EnumCard::MARKET },
	{ "handelshuis", EnumCard::TRADING_POST },
	{ "kasteel", EnumCard::CASTLE },
	{ "paleis", EnumCard::PALACE },
	{ "winkels", EnumCard::STORES },
	{ "raadhuis", EnumCard::CITY_HALL },
	{ "haven", EnumCard::HARBOUR },
	{ "tempel", EnumCard::TEMPLE },
	{ "kerk", EnumCard::CHURCH },
	{ "kathedraal", EnumCard::CATHEDRAL },
	{ "landgoed", EnumCard::ESTATE },
	{ "klooster", EnumCard::MONASTERY },
	{ "wachttoren", EnumCard::WATCHTOWER },
	{ "gevangenis", EnumCard::PRISON },
	{ "toernooiveld", EnumCard::BATTLEFIELD },
	{ "burcht", EnumCard::FORTRESS }
};

const std::map<EnumCard, Range> EnumCardToRange = {
	{ EnumCard::ESTATE, Range(1, 5) },
	{ EnumCard::CASTLE, Range(6, 9) },
	{ EnumCard::PALACE, Range(10, 12) },
	{ EnumCard::TAVERN , Range(13, 17) },
	{ EnumCard::MARKET, Range(18, 21) },
	{ EnumCard::STORES, Range(22, 24) },
	{ EnumCard::TRADING_POST, Range(25, 27) },
	{ EnumCard::HARBOUR, Range(28, 30) },
	{ EnumCard::CITY_HALL, Range(31, 32) },
	{ EnumCard::TEMPLE, Range(33, 35) },
	{ EnumCard::CHURCH, Range(36, 38) },
	{ EnumCard::MONASTERY, Range(39, 41) },
	{ EnumCard::CATHEDRAL, Range(42, 43) },
	{ EnumCard::WATCHTOWER, Range(44, 46) },
	{ EnumCard::PRISON, Range(47, 49) },
	{ EnumCard::BATTLEFIELD, Range(50, 52) },
	{ EnumCard::FORTRESS, Range(53, 54) }
};