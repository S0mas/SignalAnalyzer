#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include "../WizardFramework/Common/include/gui/ChannelsSelectionView.h"
#include "../WizardFramework/Common/include/gui/ChannelStatus.h"
#include "DataGenerator.h"
#include "SignalDataSource.h"
#include "DataAcquisitor.h"
#include "DataEmitter.h"
#include "Defines.h"
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QRadioButton>
#include <QDebug>
#include <vector>

class DataController : public QObject {
	Q_OBJECT
	bool isGeneratorSelected;
	QThread generatorThread;
	DataGenerator dataGen_;
	DataAcquisitor dataAcq_;
	DataAcquisitor customDataSources_;
	std::vector<DataEmitter*> dataEmitters_;

	std::map<QString, SignalDataSource*> sources_;

	void makeConnections() {
		connect(&dataAcq_, &DataAcquisitor::logMsg, [this](QString const& msg) {emit logMsg(msg); });
	}

	std::vector<double> generateDataSingleLine() const noexcept {
		return dataGen_.generateRandomData(1000, 1, 5, 15);
	}

	std::vector<double> generateDataVec(const long long int max) const noexcept {
		return dataGen_.generateRandomData(1000, max, 30, 90);
	}

	auto dataGetter(QString const& deviceId, const uint32_t channelId) noexcept {
		return [this, deviceId, channelId]() { return realTimeData(deviceId, channelId); };
	}

	auto dataGetter(QString const& deviceId, const std::vector<uint32_t>& channelsIds) noexcept {
		return [this, deviceId, channelsIds]() { return realTimeData(deviceId, channelsIds); };
	}

	std::pair<std::vector<double>, std::vector<Timestamp6991>> realTimeData(QString const& deviceId, const uint32_t channelId) noexcept {
		return dataAcq_.data(deviceId, channelId);
	}

	std::pair<std::vector<double>, std::vector<Timestamp6991>> realTimeData(QString const& deviceId, const std::vector<uint32_t>& channelIds) noexcept {
		return dataAcq_.data(deviceId, channelIds);
	}
public:
	DataController(QObject* parent = nullptr) : QObject(parent) {
		connect(&dataAcq_, &DataAcquisitor::logMsg, this, &DataController::logMsg);
	}

	QStringList connectedDevices() const noexcept {
		auto devices = dataAcq_.devIds();
		for(auto const& pair : sources_)
			devices.append(pair.first);
		return devices;
	}

	ChannelStatuses& statuses(QString const& deviceId) noexcept {
		if (dataAcq_.hasId(deviceId))
			return dataAcq_.statuses(deviceId);
		else
			return sources_[deviceId]->statuses();
	}

	DeviceType deviceType(QString const& deviceId) const noexcept {
		if (dataAcq_.hasId(deviceId))
			return dataAcq_.deviceType(deviceId);
		else
			return sources_.at(deviceId)->deviceType();
	}

	DataEmitter* createDataEmitter(QString const& deviceId, const uint32_t channelId) noexcept {
		dataEmitters_.push_back(new DataEmitter(dataGetter(deviceId, channelId), this));
		return dataEmitters_.back();
	}

	DataEmitter* createDataEmitter(QString const& deviceId, const std::vector<uint32_t>& channelIds) noexcept {
		dataEmitters_.push_back(new DataEmitter(dataGetter(deviceId, channelIds), this));
		return dataEmitters_.back();
	}

	void setScansToDisplayStep(uint32_t const step) noexcept {
		dataAcq_.setScansToDisplayStep(step);
	}

	uint32_t scansToDisplayStep() const noexcept {
		return dataAcq_.scansToDisplayStep();
	}

	void setMaximumDynamicSignalLength(uint32_t const size) noexcept {
		dataAcq_.setQueuesSize(size);
	}

	uint32_t maximumDynamicSignalLength() const noexcept {
		return dataAcq_.queuesSize();
	}

	auto staticData(QString const& deviceId, const uint32_t channelId, uint32_t const maxSamplesNo = 0, uint32_t const startingSampleId = 0) const noexcept {
		return  sources_.at(deviceId)->data(channelId, maxSamplesNo, startingSampleId);
	}

	auto staticData(QString const& deviceId, const std::vector<uint32_t>& channelIds, uint32_t const maxSamplesNo = 0, uint32_t const startingSampleId = 0) const noexcept {
		return sources_.at(deviceId)->data(channelIds, maxSamplesNo, startingSampleId);
	}

	template<typename ScanType>
	void loadDataFromFile(QString const& fileName, QString const& deviceType) noexcept {
		sources_.insert({ fileName, new FileSignalDataSource(fileName, deviceType == "6111" ? DeviceType::_6111 : DeviceType::_6132) });
	}

	bool isRealTimeSource(QString const& deviceId) const noexcept {
		if (dataAcq_.hasId(deviceId))
			return true;
		else
			return sources_.at(deviceId)->isRealTimeSource();
	}
signals:
	void logMsg(QString const& msg) const;
};