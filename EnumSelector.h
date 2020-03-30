#pragma once
#include <QLineEdit>
#include <memory>

class EnumComboBox;
class QLabel;
class SmartEnum;

class EnumSelector : public QWidget {
	Q_OBJECT
	QLabel* label_;
	EnumComboBox* comboBox_;
	QLineEdit* lineEdit_ = new QLineEdit;
	void createConnections() const noexcept;
public:
	EnumSelector(std::unique_ptr<SmartEnum>&& smartEnum, QString const& name, QWidget* parent = nullptr);
	unsigned int value() const noexcept;
};