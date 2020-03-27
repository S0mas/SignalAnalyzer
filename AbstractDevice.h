#pragma once
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QRandomGenerator>
#include <array>
#include <vector>
#include "Command.h"
#include "Test.h"

//dbg
#include <thread>
#include <chrono>

class AbstractDevice : public QObject {
	Q_OBJECT
	QString name_;
	std::map<int, int> dataStreams_;
	std::map<int, ControlMode> controllersModes_;
	AcquisitionConfigurationDataModel acqConfigModel_;
	bool isAcqActive_ = false;
	mutable bool state = false;
	virtual bool isDeviceAvailable() const noexcept {
		state = !state;
		return state;
	}
	void updateCounters() noexcept {
		auto testErrorId = QRandomGenerator::global()->bounded(0, tests_.size());
		tests_[testErrorId].set(tests_[testErrorId].count(), tests_[testErrorId].errors() + 1);
		for (auto& test : tests_)
			test.set(test.count() + 1, test.errors());
	}
	QTimer* timer_;
	std::vector<Test> tests_;

	bool isConnected(int const id) const noexcept {
		for(auto stream : dataStreams_)
			if (stream.second == id)
				return true;
		return false;
	}
public:
	AbstractDevice(QObject* parent = nullptr) : QObject(parent) {
		dataStreams_[1] = 0;
		dataStreams_[2] = 0;
		dataStreams_[3] = 0;
		dataStreams_[4] = 0;
		timer_ = new QTimer(this);
		connect(timer_, &QTimer::timeout, 
			[this]() {
				updateCounters();
				emit sendCounters(tests_);
			}
		);
	}
public slots:
	void handleDeviceStatusReq() const noexcept {
		qDebug() << "sending status";
		isDeviceAvailable() ? emit enable() : emit disable();
	}
public slots:
	void handleCommandExecutionReq(Command const cmd) const noexcept {
		qDebug() << "Device: " << name_ << " handles: " << cmd.toString();
	}

	void handleStartTestsReq(std::vector<TestType> const& selection) noexcept {
		if (!selection.empty()) {
			qDebug() << "Tests Started";
			tests_.clear();
			for (auto type : selection)
				tests_.push_back(Test(type));
			timer_->start(500);
			emit testsStarted();
		}
	}

	void handleStopTestsReq() const noexcept {
		qDebug() << "Tests Stopped";
		timer_->stop();
		emit testsStopped();
		emit sendCounters(tests_);
	}

	int controlId() const noexcept {
		for (auto controller : controllersModes_)
			if (controller.second == ControlMode::CONTROLLER)
				return controller.first;
		return 0;
	}

	void handleChangeControlModeReq(int const id, ControlMode const mode) noexcept {
		if (isConnected(id)) {
			if (mode == ControlMode::CONTROLLER && !controlId() || mode == ControlMode::LISTENER) {
				controllersModes_[id] = mode;
				emit controlModeChanged(id, controllersModes_[id]);
			}	
		}
	}

	void handleConnect(int const id, int const port) noexcept {
		if (isConnected(id))
			emit connected(id);
		else if (dataStreams_[port] == 0) {
			for (auto i = 0; i < 100; ++i) {
				std::this_thread::sleep_for(std::chrono::milliseconds(60));
				QApplication::processEvents();
			}
			dataStreams_[port] = id;
			controllersModes_.insert({ id, ControlMode::LISTENER });
			emit connected(id);
			if (isAcqActive_)
				emit acqStarted(acqConfigModel_);
		}
	}

	void handleDisconnect(int const id) noexcept {
		for (auto& stream : dataStreams_)
			if (stream.second == id) {
				controllersModes_.erase(id);
				stream.second = 0;
			}
	}

	void handleStatusReq(int const id) const noexcept {
		if (isConnected(id)) {
			Status s;
			s.id_ = id;
			s.mode_ = controllersModes_.at(id);
			s.state_ = "IDLE";
			for (auto stream : dataStreams_)
				s.streams_[stream.first] = stream.second == 0;
			emit status(s);
		}
	}

	void handleStartAcqReq(int const id, AcquisitionConfigurationDataModel const& configModel) noexcept {
		if (id == controlId()) {
			qDebug() << "acq started";
			isAcqActive_ = true;
			acqConfigModel_ = configModel;
			emit acqStarted(acqConfigModel_);
		}
	}

	void handleStopAcqReq(int const id) noexcept {
		if (id == controlId() && isAcqActive_) {
			qDebug() << "acq stopped";
			isAcqActive_ = false;
			emit acqStopped();
		}
	}
signals:
	void disable() const;
	void enable() const;
	void testsStarted() const;
	void testsStopped() const;
	void sendCounters(std::vector<Test> const& tests) const;
	void controlModeChanged(unsigned int const id, ControlMode const mode) const;
	void connected(int const id) const;
	void dataStreamsState() const;
	void status(Status const status) const;
	void acqStarted(AcquisitionConfigurationDataModel const& confifgModel) const;
	void acqStopped() const;
};
