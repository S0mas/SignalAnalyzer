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

class FrontendController : public QGroupBox {
	Q_OBJECT
	QStateMachine sm_;
signals:
	void commandExecutionReq(Command const cmd) const;
private:
	void initializeStateMachine(AbstractDevice* device, QLineEdit* lineEdit, QCheckBox* checkBox) noexcept {
		auto enabled = new QState();
		auto active = new QState(enabled);
		auto inactive = new QState(enabled);
		auto disabled = new QState();

		enabled->setInitialState(inactive);
		auto checkTransition = new CheckedTransition(checkBox);
		checkTransition->setTargetState(active);
		inactive->addTransition(checkTransition);

		auto unchecktransition = new UncheckedTransition(checkBox);
		unchecktransition->setTargetState(inactive);
		active->addTransition(unchecktransition);

		enabled->addTransition(device, &AbstractDevice::disable, disabled);
		disabled->addTransition(device, &AbstractDevice::enable, enabled);

		sm_.addState(enabled);
		sm_.addState(disabled);

		connect(enabled, &QState::entered,
			[this]() {
				setEnabled(true);
			}
		);
		connect(active, &QState::entered,
			[lineEdit]() {
				lineEdit->setEnabled(true);
			}
		);
		connect(inactive, &QState::entered,
			[lineEdit]() {
				lineEdit->setEnabled(false);
			}
		);
		connect(disabled, &QState::entered,
			[this, checkBox]() {
				setEnabled(false);
				checkBox->setChecked(false);
			}
		);
		sm_.setInitialState(disabled);
		sm_.start();
	}
public:
	FrontendController(AbstractDevice* device, int const index, QWidget* parent = nullptr) : QGroupBox(QString("Front End %1").arg(index), parent) {
		auto lineEdit = new QLineEdit;
		lineEdit->setMaximumWidth(70);
		lineEdit->setInputMask("\\0\\xHHHHHHHH;0");
		lineEdit->setEnabled(false);

		auto checkBox = new QCheckBox;
		checkBox->setChecked(false);

		auto layout = new QHBoxLayout;
		layout->addWidget(lineEdit);
		layout->addWidget(checkBox);
		setLayout(layout);

		connect(this, &FrontendController::sendCommand,
			[this, lineEdit, checkBox](unsigned int const cmd, unsigned int const address) {
				if (isEnabled() && checkBox->isChecked())
					emit commandExecutionReq({ cmd , address, lineEdit->text().toUInt(nullptr, 16) });
			}
		);
		connect(this, &FrontendController::stateReq, device, &AbstractDevice::handleDeviceStatusReq);
		connect(this, &FrontendController::commandExecutionReq, device, &AbstractDevice::handleCommandExecutionReq);

		initializeStateMachine(device, lineEdit, checkBox);
	}

signals:
	void sendCommand(unsigned int const cmd, unsigned int const address) const;
	void stateReq() const;
};

class RegisterController : public QGroupBox {
	Q_OBJECT
public:
	RegisterController(AbstractDevice* device1, AbstractDevice* device2, QWidget* parent = nullptr) : QGroupBox("Register Controller", parent) {
		auto commandSelector = Selector::createSelector(Category(cmdTypes), "Command");
		auto addressSelector = Selector::createSelector(Category(regTypes), "Address");

		auto dataLabel = new QLabel("Data");
		auto frontend1 = new FrontendController(device1, 1);
		auto frontend2 = new FrontendController(device2, 2);

		auto refreshButton = new QPushButton("Refresh");
		auto executeButton = new QPushButton("Execute");

		auto dataRow = new QHBoxLayout;
		dataRow->addWidget(frontend1);
		dataRow->addWidget(frontend2);

		auto acceptRow = new QHBoxLayout;
		acceptRow->addWidget(refreshButton);
		acceptRow->addStretch(0);
		acceptRow->addWidget(executeButton, 1, Qt::AlignRight);

		auto layout = new QVBoxLayout;
		layout->addWidget(commandSelector);
		layout->addWidget(addressSelector);
		layout->addLayout(dataRow);
		layout->addLayout(acceptRow);
		setLayout(layout);

		connect(refreshButton, &QPushButton::clicked,
			[frontend1, frontend2]() {
				emit frontend1->stateReq();
				emit frontend2->stateReq();
			}
		);

		connect(executeButton, &QPushButton::clicked, 
			[this, commandSelector, addressSelector, frontend1, frontend2]() {
				emit frontend1->sendCommand(commandSelector->value(), addressSelector->value());
				emit frontend2->sendCommand(commandSelector->value(), addressSelector->value());
			}
		);
	}
};