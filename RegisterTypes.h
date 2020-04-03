#pragma once
#include "../WizardFramework/Common/include/SmartEnum.h"

class RegistersTypes : public SmartEnum {
public:
	enum Types {
		REG_1,
		REG_2,
		REG_3,
	};

	RegistersTypes() : SmartEnum({ REG_1 , REG_2, REG_3 }, true) {}
	QString toString(int const reg) const noexcept override {
		switch (reg) {
		case REG_1:
			return "REG_1";
		case REG_2:
			return "REG_2";
		case REG_3:
			return "REG_3";
		default:
			return SmartEnum::toString(reg);
		}
	}

	int fromString(QString const& str) const noexcept override {
		if (str == "REG_1")
			return REG_1;
		else if (str == "REG_2")
			return REG_2;
		else if (str == "REG_3")
			return REG_3;
		else
			return SmartEnum::fromString(str);
	}

	unsigned int toUInt(int const reg) const noexcept override {
		switch (reg) {
		case REG_1:
			return 0x1;
		case REG_2:
			return 0x2;
		case REG_3:
			return 0x3;
		default:
			return 0x0;
		}
	}

	int inputWidth() const noexcept override {
		return 4;
	}
};