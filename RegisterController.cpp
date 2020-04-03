#include "RegisterController.h"

void TargetFrontendCardView::createConnections(AbstractDeviceXX* device) noexcept {
	connect(this, &TargetFrontendCardView::sendCommand,
		[this](unsigned int const cmd, unsigned int const address) {
			if (isEnabled() && checkBox_->isChecked())
				emit commandExecutionReq({ cmd , address, lineEdit_->text().toUInt(nullptr, 16) });
		}
	);
	connect(this, &TargetFrontendCardView::stateReq, device, &AbstractDeviceXX::handleDeviceStatusReq);
	connect(this, &TargetFrontendCardView::commandExecutionReq, device, &AbstractDeviceXX::handleCommandExecutionReq);

	initializeStateMachine(device);
}

void TargetFrontendCardView::initializeStateMachine(AbstractDeviceXX* device) noexcept {
	auto enabled = new QState();
	auto active = new QState(enabled);
	auto inactive = new QState(enabled);
	auto disabled = new QState();

	enabled->setInitialState(inactive);
	auto checkTransition = new CheckedTransition(checkBox_);
	checkTransition->setTargetState(active);
	inactive->addTransition(checkTransition);

	auto unchecktransition = new UncheckedTransition(checkBox_);
	unchecktransition->setTargetState(inactive);
	active->addTransition(unchecktransition);

	enabled->addTransition(device, &AbstractDeviceXX::disable, disabled);
	disabled->addTransition(device, &AbstractDeviceXX::enable, enabled);

	sm_.addState(enabled);
	sm_.addState(disabled);

	connect(enabled, &QState::entered,
		[this]() {
			setEnabled(true);
		}
	);
	connect(active, &QState::entered,
		[this]() {
			lineEdit_->setEnabled(true);
		}
	);
	connect(inactive, &QState::entered,
		[this]() {
			lineEdit_->setEnabled(false);
		}
	);
	connect(disabled, &QState::entered,
		[this]() {
			setEnabled(false);
			checkBox_->setChecked(false);
		}
	);
	sm_.setInitialState(disabled);
	sm_.start();
}

TargetFrontendCardView::TargetFrontendCardView(AbstractDeviceXX* device, int const index, QWidget* parent) : QGroupBox(QString("Front End %1").arg(index), parent) {
	lineEdit_->setMaximumWidth(70);
	lineEdit_->setInputMask("\\0\\xHHHHHHHH;0");
	lineEdit_->setEnabled(false);

	checkBox_->setChecked(false);

	auto layout = new QHBoxLayout;
	layout->addWidget(lineEdit_);
	layout->addWidget(checkBox_);
	setLayout(layout);

	createConnections(device);
}

void RegisterController::createConnections(AbstractDeviceXX* device1, AbstractDeviceXX* device2) noexcept {
	connect(refreshButton, &QPushButton::clicked, frontend1_, &TargetFrontendCardView::stateReq);
	connect(refreshButton, &QPushButton::clicked, frontend2_, &TargetFrontendCardView::stateReq);
	connect(executeButton, &QPushButton::clicked,
		[this]() {
			emit frontend1_->sendCommand(commandSelector_->value(), addressSelector_->value());
			emit frontend2_->sendCommand(commandSelector_->value(), addressSelector_->value());
		}
	);
}

RegisterController::RegisterController(AbstractDeviceXX* device1, AbstractDeviceXX* device2, QWidget* parent)
	: QGroupBox("Register Controller", parent), frontend1_(new TargetFrontendCardView(device1, 1)), frontend2_(new TargetFrontendCardView(device2, 2)){
	auto frontendsLayout = new QHBoxLayout;
	frontendsLayout->addWidget(frontend1_);
	frontendsLayout->addWidget(frontend2_);

	auto acceptRow = new QHBoxLayout;
	acceptRow->addWidget(refreshButton);
	acceptRow->addStretch(0);
	acceptRow->addWidget(executeButton, 1, Qt::AlignRight);

	auto layout = new QVBoxLayout;
	layout->addWidget(commandSelector_);
	layout->addWidget(addressSelector_);
	layout->addLayout(frontendsLayout);
	layout->addLayout(acceptRow);
	setLayout(layout);

	createConnections(device1, device2);
}