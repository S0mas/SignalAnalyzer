#include "Controller6991.h"

void Controller6991::createConnections(AbstractDevice* device) noexcept {
	connect(this, &Controller6991::connected, connectDisconnectButton_, &TwoStateButton::connected);
	connect(this, &Controller6991::disconnectReq, connectDisconnectButton_, &TwoStateButton::disconnected);
	connect(device, &AbstractDevice::acqStarted, acqStartStopButton_, &TwoStateButton::connected);
	connect(device, &AbstractDevice::acqStopped, acqStartStopButton_, &TwoStateButton::disconnected);
	connect(device, &AbstractDevice::controlModeChanged,
		[this](int const id, ControlMode const mode) {
			if (id == id_) {
				if (mode == ControlMode::CONTROLLER)
					emit changeToController();
				else if (mode == ControlMode::LISTENER)
					emit changeToListener();
			}
		}
	);
	connect(device, &AbstractDevice::status,
		[this](Status const& status) {
			if (status.id_ == id_)
				statusView_->update(status);
		}
	);
	connect(device, &AbstractDevice::connected,
		[this](int const id) {
			if (id == id_)
				emit connected();
		}
	);
	connect(device, &AbstractDevice::acqStarted, this, &Controller6991::setModel);
	connect(this, &Controller6991::changeControlModeReq, device, &AbstractDevice::handleChangeControlModeReq);
	connect(this, &Controller6991::connectReq, device, &AbstractDevice::handleConnect);
	connect(this, &Controller6991::disconnectReq, device, &AbstractDevice::handleDisconnect);
	connect(this, &Controller6991::statusReq, device, &AbstractDevice::handleStatusReq);
	connect(this, &Controller6991::startAcqReq, device, &AbstractDevice::handleStartAcqReq);
	connect(this, &Controller6991::stopAcqReq, device, &AbstractDevice::handleStopAcqReq);
	connect(setModeButton_, &QPushButton::clicked, [this]() { emit changeControlModeReq(id_, static_cast<ControlMode>(comboBoxMode_->currentData().toInt())); });

	auto autoRefreshTimer = new QTimer(this);
	connect(autoRefreshTimer, &QTimer::timeout, [this]() { emit statusReq(id_); });
	connect(statusAutoRefreshCheckBox_, &QCheckBox::stateChanged,
		[autoRefreshTimer](int const state) {
			state == Qt::Checked ? autoRefreshTimer->start(500) : autoRefreshTimer->stop();
		}
	);
	connect(resfreshButton_, &QPushButton::clicked, [this]() { emit statusReq(id_); });
	initializeStateMachine(device);
}

void Controller6991::initializeStateMachine(AbstractDevice* device) noexcept {
	auto connected = new QState();
	auto disconnected = new QState();
	auto controller = new QState(connected);
	auto listener = new QState(connected);

	connected->setInitialState(listener);

	listener->addTransition(this, &Controller6991::changeToController, controller);
	controller->addTransition(this, &Controller6991::changeToListener, listener);
	disconnected->addTransition(this, &Controller6991::connected, listener);
	connected->addTransition(this, &Controller6991::disconnectReq, disconnected);

	sm_.addState(connected);
	sm_.addState(disconnected);

	connect(disconnected, &QState::entered,
		[this, device]() {
			modeGroup_->setEnabled(false);
			comboBoxMode_->setCurrentIndex(0);
			dataStreamComboBox_->setEnabled(true);
			resfreshButton_->setEnabled(false);
			clockSourceGroup_->setEnabled(false);
			scanRateView_->setEnabled(false);
			startModeView_->setEnabled(false);
			stopModeView_->setEnabled(false);
			acqStartStopButton_->setEnabled(false);
			statusAutoRefreshCheckBox_->setEnabled(false);
			statusAutoRefreshCheckBox_->setChecked(false);
			dataStorageCheckBox_->setEnabled(false);
			dataStorageCheckBox_->setChecked(false);

		}
	);
	connect(connected, &QState::entered,
		[this]() {
			modeGroup_->setEnabled(true);
			resfreshButton_->setEnabled(true);
			dataStorageCheckBox_->setEnabled(true);
			statusAutoRefreshCheckBox_->setEnabled(true);
			dataStreamComboBox_->setEnabled(false);
		}
	);
	connect(controller, &QState::entered,
		[this]() {
			clockSourceGroup_->setEnabled(true);
			scanRateView_->setEnabled(true);
			startModeView_->setEnabled(true);
			stopModeView_->setEnabled(true);
			acqStartStopButton_->setEnabled(true);
		}
	);
	connect(controller, &QState::exited,
		[this]() {
			clockSourceGroup_->setEnabled(false);
			scanRateView_->setEnabled(false);
			startModeView_->setEnabled(false);
			stopModeView_->setEnabled(false);
			acqStartStopButton_->setEnabled(false);
		}
	);
	sm_.setInitialState(disconnected);
	sm_.start();
}

AcquisitionConfigurationDataModel Controller6991::model() const noexcept {
	AcquisitionConfigurationDataModel request;
	request.startModeModel_ = startModeView_->model();
	request.stopModeModel_ = stopModeView_->model();
	request.scanRateModel_ = scanRateView_->model();
	request.clockSource_ = static_cast<ClockSource>(clockSourceComboBox->currentData().toInt());
	return request;
}

void Controller6991::setModel(AcquisitionConfigurationDataModel const& model) noexcept {
	startModeView_->setModel(model.startModeModel_);
	stopModeView_->setModel(model.stopModeModel_);
	scanRateView_->setModel(model.scanRateModel_);
	clockSourceComboBox->setCurrentIndex(static_cast<int>(model.clockSource_));
}

Controller6991::Controller6991(AbstractDevice * device, unsigned int const id, QWidget * parent) : QGroupBox("Controller", parent), id_(id) {
	comboBoxMode_->setMaximumWidth(100);
	for (auto mode : CONTROL_MODES)
		comboBoxMode_->addItem(toString(mode), static_cast<int>(mode));

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(comboBoxMode_);
	hlayout->addWidget(setModeButton_);
	modeGroup_->setLayout(hlayout);

	dataStreamComboBox_->setMaximumWidth(30);
	dataStreamComboBox_->addItems(streams);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(dataStreamComboBox_);
	hlayout->addWidget(connectDisconnectButton_);
	dataStreamGroup_->setLayout(hlayout);

	for (auto clock : CLOCK_SOURCES)
		clockSourceComboBox->addItem(toString(clock), static_cast<int>(clock));

	auto vlayout = new QVBoxLayout;
	vlayout->addWidget(clockSourceComboBox);
	clockSourceGroup_->setLayout(vlayout);

	vlayout = new QVBoxLayout;
	hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(modeGroup_);
	hlayout->addWidget(dataStreamGroup_);
	vlayout->addLayout(hlayout);
	hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(scanRateView_);
	hlayout->addWidget(clockSourceGroup_);
	vlayout->addLayout(hlayout);
	vlayout->addWidget(startModeView_);
	vlayout->addWidget(stopModeView_);
	vlayout->addWidget(dataStorageCheckBox_);
	vlayout->addWidget(statusAutoRefreshCheckBox_);

	auto hButtonslayout = new QHBoxLayout;
	hButtonslayout->addWidget(resfreshButton_);
	hButtonslayout->addWidget(acqStartStopButton_, 1, Qt::AlignRight);

	auto layout = new QVBoxLayout;
	layout->addLayout(vlayout);
	layout->addWidget(statusView_);
	layout->addLayout(hButtonslayout);
	setLayout(layout);

	createConnections(device);
}