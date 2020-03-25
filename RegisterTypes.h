#pragma once
#include <QString>
#include <vector>

enum class RegistersTypes {
	REG_1,
	REG_2,
	REG_3,
	CUSTOM
};

inline std::vector<RegistersTypes> regTypes = { RegistersTypes::REG_1 , RegistersTypes::REG_2, RegistersTypes::REG_3, RegistersTypes::CUSTOM };
inline QString toString(RegistersTypes const reg) noexcept {
	switch (reg) {
	case RegistersTypes::REG_1:
		return "REG_1";
	case RegistersTypes::REG_2:
		return "REG_2";
	case RegistersTypes::REG_3:
		return "REG_3";
	default:
		return "CUSTOM";
	}
}

inline unsigned int toUInt(RegistersTypes const reg) noexcept {
	switch (reg) {
	case RegistersTypes::REG_1:
		return 0x1;
	case RegistersTypes::REG_2:
		return 0x2;
	case RegistersTypes::REG_3:
		return 0x3;
	default:
		return 0x0;
	}
}

inline int inputWidth(RegistersTypes const reg) noexcept {
	return 4;
}