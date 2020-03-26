#include "RegisterController.h"

void FrontendCardController::initializeStateMachine(AbstractDevice* device, QLineEdit* lineEdit, QCheckBox* checkBox) noexcept {
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

FrontendCardController::FrontendCardController(AbstractDevice* device, int const index, QWidget* parent) : QGroupBox(QString("Front End %1").arg(index), parent) {
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

	connect(this, &FrontendCardController::sendCommand,
		[this, lineEdit, checkBox](unsigned int const cmd, unsigned int const address) {
			if (isEnabled() && checkBox->isChecked())
				emit commandExecutionReq({ cmd , address, lineEdit->text().toUInt(nullptr, 16) });
		}
	);
	connect(this, &FrontendCardController::stateReq, device, &AbstractDevice::handleDeviceStatusReq);
	connect(this, &FrontendCardController::commandExecutionReq, device, &AbstractDevice::handleCommandExecutionReq);

	initializeStateMachine(device, lineEdit, checkBox);
}



RegisterController::RegisterController(AbstractDevice* device1, AbstractDevice* device2, QWidget* parent) : QGroupBox("Register Controller", parent) {
	auto commandSelector = Selector::createSelector(Category(cmdTypes), "Command");
	auto addressSelector = Selector::createSelector(Category(regTypes), "Address");

	auto dataLabel = new QLabel("Data");
	auto frontend1 = new FrontendCardController(device1, 1);
	auto frontend2 = new FrontendCardController(device2, 2);

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