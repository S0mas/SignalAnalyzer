#include "Controller6991.h"

void Controller6991::createConnections() noexcept {
	connect(deviceIF_, &Device6991::acquisitionStarted, acqStartStopButton_, &TwoStateButton::connected);
	connect(deviceIF_, &Device6991::acquisitionStopped, acqStartStopButton_, &TwoStateButton::disconnected);
	connect(deviceIF_, &Device6991::connectedDataStream, connectDisconnectButton_, &TwoStateButton::connected);
	connect(deviceIF_, &Device6991::disconnectedDataStream, connectDisconnectButton_, &TwoStateButton::disconnected);
	connect(deviceIF_, &Device6991::state, statusView_, &StatusView::update);
	connect(deviceIF_, &Device6991::configuration, this, &Controller6991::setModel);

	connect(setModeButton_, &QPushButton::clicked,
		[this]() {
			if (comboBoxMode_->currentData().toInt() == ControlModeEnum::CONTROLLER)
				deviceIF_->takeControlRequest(id_);
			else
				deviceIF_->releaseControlRequest();

			auto autoRefreshTimer = new QTimer(this);
			connect(autoRefreshTimer, &QTimer::timeout, deviceIF_, &Device6991::deviceStateRequest);
			connect(statusAutoRefreshCheckBox_, &QCheckBox::stateChanged,
				[autoRefreshTimer](int const state) {
					state == Qt::Checked ? autoRefreshTimer->start(500) : autoRefreshTimer->stop();
				}
			);
			connect(resfreshButton_, &QPushButton::clicked, deviceIF_, &Device6991::deviceStateRequest);
		}
	);
	initializeStateMachine();
}

void Controller6991::initializeStateMachine() noexcept {
	auto connected = new QState();
	auto disconnected = new QState();
	auto controller = new QState(connected);
	auto listener = new QState(connected);

	connected->setInitialState(listener);

	listener->addTransition(deviceIF_, &Device6991::controlGranted, controller);
	controller->addTransition(deviceIF_, &Device6991::controlReleased, listener);
	disconnected->addTransition(deviceIF_, &Device6991::connectedDataStream, listener);
	connected->addTransition(deviceIF_, &Device6991::disconnectedDataStream, disconnected);

	sm_.addState(connected);
	sm_.addState(disconnected);

	connect(disconnected, &QState::entered,
		[this]() {
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

Configuration6991 Controller6991::model() const noexcept {
	Configuration6991 config;
	config.scanRate_ = scanRateView_->model();
	config.startMode_ = startModeView_->model();
	config.stopMode_ = stopModeView_->model();
	config.clockSource_ = static_cast<ClockSourceEnum::Type>(clockSourceComboBox->currentData().toInt());
	//config.timestamps_ =
	//config.scansPerDirectReadPacket_ = 
	//config.fansMode_ =
	return config;
}

void Controller6991::setModel(Configuration6991 const& configuration) noexcept {
	if (configuration.scanRate_)
		scanRateView_->setModel(*configuration.scanRate_);
	startModeView_->setModel(configuration.startMode_);
	stopModeView_->setModel(configuration.stopMode_);
	if (configuration.clockSource_)
		clockSourceComboBox->setCurrentIndex(*configuration.clockSource_);

	//if (configuration.fansMode_)
	//	//
	//if (configuration.timestamps_)
	//	//
	//if (configuration.scansPerDirectReadPacket_)
	//	//
	//if (configuration.ptpTime_)
	//	//
}

Controller6991::Controller6991(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, unsigned int const id, QWidget * parent) : QGroupBox("Controller", parent), id_(id) {
	deviceIF_ = new Device6991("Device6111", hwConnector, scpiIF, 256, this);

	comboBoxMode_->setMaximumWidth(100);
	for (auto mode : ControlModeEnum::TYPES)
		comboBoxMode_->addItem(ControlModeEnum::toString(mode), mode);

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

	for (auto clock : ClockSourceEnum::TYPES)
		clockSourceComboBox->addItem(ClockSourceEnum::toString(clock), clock);

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

	createConnections();
}