#ifndef ENUMSTATE
#define ENUMSTATE

#include <map>

enum class EnumState {
	UNSTARTED,
	SETUP
};

const std::map<std::string, EnumState> convertToEnumState = {
	{"Pre-Setup", EnumState::UNSTARTED},
	{"Setup", EnumState::SETUP}
};

#endif
