#pragma once
#include <QString>
#include <vector>
#include "Defines.h"

template<typename T>
class Category {
	std::vector<T> types_;
public:
	Category(std::vector<T> const& types) : types_(types) {}

	auto type(int const index) const noexcept {
		return types_[index];
	}

	auto types() const noexcept {
		return types_;
	}

	auto customItemIndex() const noexcept {
		return types_.size() - 1;
	}

	QString toHexString(int const index = 0) const noexcept {
		return toHex(type(index));
	}

	unsigned int toUInt(int const index = 0) const noexcept {
		return toUInt(type(index));
	}

	QString toString(int const index = 0) const noexcept {
		return toString(type(index));
	}

	QString inputMask() const noexcept {
		return ::inputMask<T>();
	}
};