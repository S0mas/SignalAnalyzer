#pragma once
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>

#include "AbstractDevice.h"
#include "Category.h"
#include "CommandTypes.h"
#include "Command.h"
#include "RegisterTypes.h"
#include "CustomTransitions.h"

class Selector : public QWidget {
	Q_OBJECT
	template<typename T>
	static QLayout* createSelectionWidget(Category<T> const& category, QString const& name, Selector* selector) {
		auto label = new QLabel(name);
		label->setMinimumWidth(50);
		label->setMaximumWidth(50);
		auto comboBox = new QComboBox;
		for (auto type : category.types())
			comboBox->addItem(toString(type));
		auto lineEdit = new QLineEdit(category.toHexString());
		lineEdit->setMaximumWidth(50);
		lineEdit->setDisabled(true);
		lineEdit->setInputMask(category.inputMask());
		selector->value_ = lineEdit->text();

		auto layout = new QHBoxLayout;
		layout->addWidget(label);
		layout->addWidget(comboBox);
		layout->addWidget(lineEdit);

		QObject::connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[category, lineEdit](int const index) {
				lineEdit->setText(category.toHexString(index));
				index == category.customItemIndex() ? lineEdit->setEnabled(true) : lineEdit->setDisabled(true);
			}
		);

		QObject::connect(lineEdit, &QLineEdit::textChanged, [selector](QString const& text) { selector->value_ = text; });

		return layout;
	}
	Selector(QWidget* parent = nullptr) : QWidget(parent) {}
	QString value_;
public:
	template<typename T>
	static Selector* createSelector(Category<T> const& category, QString const& name, QWidget* parent = nullptr) {
		auto selector = new Selector(parent);
		selector->setLayout(createSelectionWidget(category, name, selector));
		return selector;
	}

	QString valueStr() const noexcept {
		return value_;
	}

	unsigned int value() const noexcept {
		return value_.toUInt(nullptr, 16);
	}
};

class FrontendCardController : public QGroupBox {
	Q_OBJECT
	QStateMachine sm_;
signals:
	void commandExecutionReq(Command const cmd) const;
private:
	void initializeStateMachine(AbstractDevice* device, QLineEdit* lineEdit, QCheckBox* checkBox) noexcept;
public:
	FrontendCardController(AbstractDevice* device, int const index, QWidget* parent = nullptr);
signals:
	void sendCommand(unsigned int const cmd, unsigned int const address) const;
	void stateReq() const;
};

class RegisterController : public QGroupBox {
	Q_OBJECT
public:
	RegisterController(AbstractDevice* device1, AbstractDevice* device2, QWidget* parent = nullptr);
};