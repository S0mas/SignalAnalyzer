#pragma once
#include <memory>
#include <vector>
#include <QString>
#include <QComboBox>
#include <QWidget>

class SmartEnum {
	std::vector<int> types_;
	const int CUSTOM = -1;
	bool isCustomEnabled_;
public:
	SmartEnum(std::vector<int> const& types, bool const customEnabled = false) noexcept : types_(types), isCustomEnabled_(customEnabled) {
		if (isCustomEnabled_)
			types_.push_back(CUSTOM); // add element that indicates custom element is selected
	}
	virtual ~SmartEnum() = default;

	virtual QString toString(int const concreteEnum) const noexcept {
		return isCustomEnabled_ ? "CUSTOM" : "ERROR_INVALID_ENUM_VALUE";
	}
	virtual unsigned int toUInt(int const concreteEnum) const noexcept = 0;
	virtual int inputWidth() const noexcept = 0;

	int size() const noexcept {
		return types_.size();
	}

	auto begin() const noexcept {
		return types_.begin();
	}

	auto end() const noexcept {
		return types_.end();
	}

	QString inputMask() const noexcept {
		return "\\0\\x" + QString(inputWidth(), 'H');
	}

	QString toHex(int const concreteEnum) const noexcept {
		return QString("0x%1").arg(toUInt(concreteEnum), inputWidth(), 16, QLatin1Char('0'));
	}

	bool isCustomEnabled() const noexcept {
		return isCustomEnabled_;
	}
};


class EnumComboBox : public QComboBox {
	Q_OBJECT
	std::unique_ptr<SmartEnum> smartEnum_;

public:
	EnumComboBox(std::unique_ptr<SmartEnum>& smartEnum, QWidget* parent = nullptr) : QComboBox(parent), smartEnum_(std::move(smartEnum)) {
		for (auto concreteEnum : *smartEnum_)
			addItem(smartEnum_->toString(concreteEnum), concreteEnum);
	}

	QString toHex() const noexcept {
		return smartEnum_->toHex(currentData().toInt());
	}

	unsigned int toUint() const noexcept {
		return smartEnum_->toUInt(currentData().toInt());
	}

	QString inputMask() const noexcept {
		return smartEnum_->inputMask();
	}

	bool isCustomSelected() const noexcept {
		return smartEnum_->isCustomEnabled() && currentIndex() == smartEnum_->size() - 1;
	}
};