#pragma once
#include <QString>
#include "Defines.h"

class Command {
	unsigned int cmdValue_;
	unsigned int registerAddress_;
	unsigned int data_;
public:
	Command(unsigned int const cmdValue, unsigned int const registerAddress, unsigned int const data) : cmdValue_(cmdValue), registerAddress_(registerAddress), data_(data) {}

	QString toString() const noexcept {
		return QString("cmd: %1 address: %2 data: %3").arg(toHex(cmdValue_, 2), toHex(registerAddress_, 4), toHex(data_, 8));
	}
};