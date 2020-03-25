#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStateMachine>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>

#include "AbstractDevice.h"
#include "Defines.h"

class Controller6991 : public QGroupBox {
	Q_OBJECT
	QStateMachine sm_;
	QTime time_;
	unsigned int id_;
	QWidget* modeGroup_;
	QWidget* dataStreamGroup_;
	QWidget* clockSourceGroup_;
	QWidget* scanRateGroup_;
	QWidget* startModeGroup_;
	QComboBox* comboBoxMode_;
	QWidget* stopModeGroup_;
	QPushButton* startButton_;
	QPushButton* stopButton_;
	QPushButton* connectDisconnectButton_;
	QPushButton* resfreshButton_;
	QCheckBox* dataStorageCheckBox_;
	QCheckBox* statusAutoRefreshCheckBox_;
	QComboBox* startModeComboBox_;
	QComboBox* dataStreamComboBox_;

	QStringList modes = { "Listener", "Controller" };
	QStringList streams = { "1", "2", "3", "4" };
	QStringList clockSources = { "PTP", "Debug", "Software" };
	QStringList startModes = { "Immediate", "PTP Alarm" };
	QStringList stopModes = { "Scans Treshold", "Time", "User Request" };
	QStringList units = { "Hz", "us" };

	int const HZ_LIMIT = 1'000'000;
	int const US_LIMIT = 1'000'000;
signals:
	void controllerModeReq(unsigned int const id) const;
	void controllerModeResignation(unsigned int const id) const;
	void controllerModeAcq() const;
	void connectReq(int const id, int const port) const;
	void disconnectReq(int const id) const;
	void connected() const;
	void handleStatus(Status const& status) const;
	void statusReq(int const id) const;
private:
	void initializeStateMachine(AbstractDevice* device) noexcept {
		auto connected = new QState();
		auto disconnected = new QState();
		auto controller = new QState(connected);
		auto listener = new QState(connected);

		connected->setInitialState(listener);

		listener->addTransition(this, &Controller6991::controllerModeAcq, controller);
		controller->addTransition(device, &AbstractDevice::controllerModeFreed, listener);
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
				scanRateGroup_->setEnabled(false);
				startModeGroup_->setEnabled(false);
				startModeComboBox_->setCurrentIndex(0);
				stopModeGroup_->setEnabled(false);
				startButton_->setEnabled(false);
				stopButton_->setEnabled(false);
				statusAutoRefreshCheckBox_->setEnabled(false);
				statusAutoRefreshCheckBox_->setChecked(false);
				dataStorageCheckBox_->setEnabled(false);
				dataStorageCheckBox_->setChecked(false);
				connectDisconnectButton_->disconnect();
				connectDisconnectButton_->setText("Connect");
				connect(connectDisconnectButton_, &QPushButton::clicked,
					[this]() {
						emit connectReq(id_, dataStreamComboBox_->currentText().toUInt());
					}
				);
			}
		);
		connect(disconnected, &QState::exited,
			[this]() {
				connectDisconnectButton_->disconnect();
				connectDisconnectButton_->setText("Disconnect");
				connect(connectDisconnectButton_, &QPushButton::clicked, [this]() {emit disconnectReq(id_); });
			}
		);
		connect(connected, &QState::entered,
			[this]() {
				modeGroup_->setEnabled(true);
				startModeComboBox_->setCurrentIndex(0);
				resfreshButton_->setEnabled(true);
				dataStorageCheckBox_->setEnabled(true);
				statusAutoRefreshCheckBox_->setEnabled(true);
				dataStreamComboBox_->setEnabled(false);
			}
		);
		connect(controller, &QState::entered,
			[this]() {
				clockSourceGroup_->setEnabled(true);
				scanRateGroup_->setEnabled(true);
				startModeGroup_->setEnabled(true);
				startModeComboBox_->setCurrentIndex(1);
				startModeComboBox_->setCurrentIndex(0);
				stopModeGroup_->setEnabled(true);
				startButton_->setEnabled(true);
				stopButton_->setEnabled(true);
			}
		);
		connect(controller, &QState::exited,
			[this]() {
				clockSourceGroup_->setEnabled(false);
				scanRateGroup_->setEnabled(false);
				startModeGroup_->setEnabled(false);
				stopModeGroup_->setEnabled(false);
				startButton_->setEnabled(false);
				stopButton_->setEnabled(false);
			}
		);
		sm_.setInitialState(disconnected);
		sm_.start();
	}

	QWidget* scanRateWidget() const noexcept {
		auto valueSpinBox = new QSpinBox;
		valueSpinBox->setMaximumWidth(130);
		valueSpinBox->setMinimumWidth(130);
		auto unitComboBox = new QComboBox;
		unitComboBox->addItems(units);
		auto layout = new QHBoxLayout;
		layout->addWidget(valueSpinBox);
		layout->addWidget(unitComboBox);

		connect(unitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[this, valueSpinBox](int const index) {
				if (index == 0) {
					valueSpinBox->setSingleStep(1);
					valueSpinBox->setRange(1, HZ_LIMIT);
					valueSpinBox->disconnect();
				}
				else {
					valueSpinBox->setSingleStep(10);
					valueSpinBox->setRange(10, US_LIMIT);
					connect(valueSpinBox, &QAbstractSpinBox::editingFinished, [valueSpinBox]() {if (valueSpinBox->value() % 10 != 0) valueSpinBox->setValue(valueSpinBox->value() - valueSpinBox->value() % 10); });
				}
			}
		);
		unitComboBox->setCurrentIndex(0);
		emit unitComboBox->currentIndexChanged(0);

		auto widget = new QGroupBox("Scan Rate");
		widget->setLayout(layout);
		return widget;
	}

	QWidget* startModeWidget() noexcept {
		startModeComboBox_ = new QComboBox;
		startModeComboBox_->setMaximumWidth(130);
		startModeComboBox_->addItems(startModes);
		auto startModeDate = new QDateTimeEdit;
		startModeDate->setDisplayFormat("dd.MM.yyyy, hh:mm:ss");
		startModeDate->setMaximumWidth(130);
		auto nowButton = new QPushButton("Now");
		connect(nowButton, &QPushButton::clicked, [startModeDate]() { startModeDate->setDateTime(QDateTime::currentDateTime()); });
		connect(startModeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[startModeDate, nowButton](int const index) {
				if (index == 0) {
					startModeDate->setEnabled(false);
					startModeDate->setDateTime(startModeDate->minimumDateTime());
					nowButton->setEnabled(false);
				}

				else {
					nowButton->setEnabled(true);
					startModeDate->setEnabled(true);
				}
			}
		);

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(startModeDate);
		hlayout->addWidget(nowButton, 1, Qt::AlignRight);
		hlayout->setContentsMargins(0, 0, 0, 0);

		auto layout = new QVBoxLayout;
		layout->addWidget(startModeComboBox_);
		layout->addLayout(hlayout);

		auto widget = new QGroupBox("Start Mode");
		widget->setLayout(layout);
		return widget;
	}

	QWidget* stopModeWidget()  noexcept {
		auto stopModeComboBox = new QComboBox;
		stopModeComboBox->setMaximumWidth(130);
		stopModeComboBox->addItems(stopModes);
		auto stopOnErrorChckBox = new QCheckBox("Stop on Error");
		auto numberOfScansLabel = new QLabel("Number of Scans");
		auto numberOfScansSpinBox = new QSpinBox;
		numberOfScansSpinBox->setRange(1, 1000000);
		numberOfScansSpinBox->setSingleStep(1);
		numberOfScansSpinBox->setMaximumWidth(130);

		auto acqusitionTimeLabel = new QLabel("Acquisition Time");
		auto acquisitionTimeEdit = new QTimeEdit;
		acquisitionTimeEdit->setDisplayFormat("hh:mm:ss:zzz");
		acquisitionTimeEdit->setMaximumWidth(130);

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(stopModeComboBox);
		hlayout->addWidget(stopOnErrorChckBox);
		hlayout->setContentsMargins(0, 0, 0, 0);
		auto layout = new QVBoxLayout;
		layout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		hlayout->addWidget(numberOfScansLabel);
		hlayout->addWidget(numberOfScansSpinBox);
		hlayout->setContentsMargins(0, 0, 0, 0);
		layout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		hlayout->addWidget(acqusitionTimeLabel);
		hlayout->addWidget(acquisitionTimeEdit);
		hlayout->setContentsMargins(0, 0, 0, 0);
		layout->addLayout(hlayout);

		connect(stopModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[numberOfScansSpinBox, acquisitionTimeEdit](int const index) {
				if (index == 0) {
					numberOfScansSpinBox->setEnabled(true);
					acquisitionTimeEdit->setTime(acquisitionTimeEdit->minimumTime());
					acquisitionTimeEdit->setEnabled(false);
				}
				else if (index == 1) {
					numberOfScansSpinBox->clear();
					numberOfScansSpinBox->setEnabled(false);
					acquisitionTimeEdit->setEnabled(true);
				}
				else {
					numberOfScansSpinBox->clear();
					numberOfScansSpinBox->setEnabled(false);
					acquisitionTimeEdit->setTime(acquisitionTimeEdit->minimumTime());
					acquisitionTimeEdit->setEnabled(false);
				}
			}
		);
		stopModeComboBox->setCurrentIndex(2);

		auto widget = new QGroupBox("Stop Mode");
		widget->setLayout(layout);
		return widget;
	}

	QWidget* statusWidget() const noexcept {
		auto idLabel = new QLabel("Id");
		auto modeLabel = new QLabel("Mode");
		auto modeValueLabel = new QLabel;
		auto controllerIdValueLabel = new QLabel(toHex(id_));
		auto dataStreamLabel = new QLabel("Data Stream");
		auto dataStreamsList = new QLabel;
		auto stateLabel = new QLabel("State");
		auto stateValueLabel = new QLabel;
		auto currentTimeLabel = new QLabel("Current time");
		auto currentTimeLabelValue = new QLabel;

		connect(this, &Controller6991::handleStatus,
			[modeValueLabel, controllerIdValueLabel, dataStreamsList, stateValueLabel, currentTimeLabelValue](Status const& status) {
				modeValueLabel->setText(status.mode_ == Mode::CONTROLLER ? "Controller" : "Listener");
				stateValueLabel->setText(status.state_);
				QString list;
				for (auto stream : status.streams_)
					stream.second ? list.append(QString("[<font color=\"green\">%1</font>]").arg(stream.first)) : list.append(QString("[<font color=\"red\">%1</font>]").arg(stream.first));
				dataStreamsList->setText(list);
				currentTimeLabelValue->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy, hh:mm:ss"));
			}
		);

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(idLabel);
		hlayout->addWidget(controllerIdValueLabel);
		auto layout = new QVBoxLayout;
		layout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		hlayout->addWidget(modeLabel);
		hlayout->addWidget(modeValueLabel);
		layout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		hlayout->addWidget(dataStreamLabel);
		hlayout->addWidget(dataStreamsList);
		layout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		hlayout->addWidget(stateLabel);
		hlayout->addWidget(stateValueLabel);
		layout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		hlayout->addWidget(currentTimeLabel);
		hlayout->addWidget(currentTimeLabelValue);
		layout->addLayout(hlayout);

		auto widget = new QGroupBox("Status");
		widget->setLayout(layout);
		return widget;
	}
public:
	Controller6991(AbstractDevice* device, unsigned int const id, QWidget* parent = nullptr) : QGroupBox("Controller", parent), id_(id) {		
		comboBoxMode_ = new QComboBox;
		comboBoxMode_->setMaximumWidth(100);
		comboBoxMode_->addItems(modes);
		auto setModeButton = new QPushButton("Set");

		modeGroup_ = new QGroupBox("Mode");
		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(comboBoxMode_);
		hlayout->addWidget(setModeButton);
		modeGroup_->setLayout(hlayout);

		dataStreamComboBox_ = new QComboBox;
		dataStreamComboBox_->setMaximumWidth(30);
		dataStreamComboBox_->addItems(streams);
		connectDisconnectButton_ = new QPushButton("Connect");

		dataStreamGroup_ = new QGroupBox("Data Stream");
		hlayout = new QHBoxLayout;
		hlayout->addWidget(dataStreamComboBox_);
		hlayout->addWidget(connectDisconnectButton_);
		dataStreamGroup_->setLayout(hlayout);

		auto clockSourceComboBox = new QComboBox;
		clockSourceComboBox->addItems(clockSources);
		clockSourceGroup_ = new QGroupBox("Clock Source");
		auto vlayout = new QVBoxLayout;
		vlayout->addWidget(clockSourceComboBox);
		clockSourceGroup_->setLayout(vlayout);

		scanRateGroup_ = scanRateWidget();
		dataStorageCheckBox_ = new QCheckBox("Data Storage");
		statusAutoRefreshCheckBox_ = new QCheckBox("Auto Status Refresh");

		startModeGroup_ = startModeWidget();
		stopModeGroup_ = stopModeWidget();
		auto statusW = statusWidget();

		startButton_ = new QPushButton("Start");
		stopButton_ = new QPushButton("Stop");
		resfreshButton_ = new QPushButton("Refresh");

		vlayout = new QVBoxLayout;
		hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->addWidget(modeGroup_);
		hlayout->addWidget(dataStreamGroup_);
		vlayout->addLayout(hlayout);
		hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->addWidget(scanRateGroup_);
		hlayout->addWidget(clockSourceGroup_);
		vlayout->addLayout(hlayout);
		vlayout->addWidget(startModeGroup_);
		vlayout->addWidget(stopModeGroup_);
		vlayout->addWidget(dataStorageCheckBox_);
		vlayout->addWidget(statusAutoRefreshCheckBox_);

		auto hButtonslayout = new QHBoxLayout;
		hButtonslayout->addWidget(resfreshButton_);
		hButtonslayout->addWidget(stopButton_, 1, Qt::AlignRight);
		hButtonslayout->addWidget(startButton_, 1, Qt::AlignRight);

		auto layout = new QVBoxLayout;
		layout->addLayout(vlayout);
		layout->addWidget(statusW);
		layout->addLayout(hButtonslayout);
		setLayout(layout);

		connect(device, &AbstractDevice::controllerModeGranted,
			[this](int const id) {
				if (id == id_)
					emit controllerModeAcq(); 
			}
		);

		connect(device, &AbstractDevice::status, 
			[this](int const id, Status const& status) {
				if (id == id_)
					handleStatus(status);
			}
		);

		connect(device, &AbstractDevice::connected,
			[this](int const id) {
				if (id == id_)
					emit connected();
			}
		);

		connect(this, &Controller6991::controllerModeReq, device, &AbstractDevice::handleControllerModeReq);
		connect(this, &Controller6991::controllerModeResignation, device, &AbstractDevice::handleControllerModeResignation);
		connect(this, &Controller6991::connectReq, device, &AbstractDevice::handleConnect);
		connect(this, &Controller6991::disconnectReq, device, &AbstractDevice::handleDisconnect);
		connect(this, &Controller6991::statusReq, device, &AbstractDevice::handleStatusReq);
		connect(setModeButton, &QPushButton::clicked,
			[this]() {
				if (comboBoxMode_->currentIndex() == 0)
					emit controllerModeResignation(id_);
				else
					emit controllerModeReq(id_);
			}
		);

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
};