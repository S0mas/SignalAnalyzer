#pragma once
#include <QString>
#include <vector>

enum class CommandsTypes {
	READ,
	WRITE,
	WRITE_LOOPBACK,
	CUSTOM
};

inline std::vector<CommandsTypes> cmdTypes = { CommandsTypes::READ , CommandsTypes::WRITE, CommandsTypes::WRITE_LOOPBACK, CommandsTypes::CUSTOM };

inline QString toString(CommandsTypes const cmd) noexcept {
	switch (cmd) {
	case CommandsTypes::READ:
		return "READ";
	case CommandsTypes::WRITE:
		return "WRITE";
	case CommandsTypes::WRITE_LOOPBACK:
		return "WRITE_LOOPBACK";
	default:
		return "CUSTOM";
	}
}

inline unsigned int toUInt(CommandsTypes const cmd) noexcept {
	switch (cmd) {
	case CommandsTypes::READ:
		return 0x51;
	case CommandsTypes::WRITE:
		return 0x52;
	case CommandsTypes::WRITE_LOOPBACK:
		return 0x53;
	default:
		return 0x00;
	}
}

inline int inputWidth(CommandsTypes const cmd) noexcept {
	return 2;
}

