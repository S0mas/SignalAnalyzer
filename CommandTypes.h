#pragma once
#include "../WizardFramework/Common/include/SmartEnum.h"

class CommandTypes : public SmartEnum {
public:
	enum Types {
		READ,
		WRITE,
		WRITE_LOOPBACK,
	};

	CommandTypes() : SmartEnum({ READ , WRITE, WRITE_LOOPBACK }, true) {}
	QString toString(int const command) const noexcept override {
		switch (command) {
		case READ:
			return "READ";
		case WRITE:
			return "WRITE";
		case WRITE_LOOPBACK:
			return "WRITE_LOOPBACK";
		default:
			return SmartEnum::toString(command);
		}
	}

	int fromString(QString const& str) const noexcept override {
		if (str == "READ")
			return READ;
		else if (str == "WRITE")
			return WRITE;
		else if (str == "WRITE_LOOPBACK")
			return WRITE_LOOPBACK;
		else
			return SmartEnum::fromString(str);
	}

	unsigned int toUInt(int const command) const noexcept override {
		switch (command) {
		case READ:
			return 0x51;
		case WRITE:
			return 0x52;
		case WRITE_LOOPBACK:
			return 0x53;
		default:
			return 0x00;
		}
	}

	int inputWidth() const noexcept override {
		return 2;
	}
};