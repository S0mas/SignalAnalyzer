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
		return [this, deviceId, channelId]() { return data(deviceId, channelId); };
	}

	auto dataGetter(QString const& deviceId, const std::vector<uint32_t>& channelsIds) noexcept {
		return [this, deviceId, channelsIds]() { return data(deviceId, channelsIds); };
	}
public:
	DataController(QObject* parent = nullptr) : QObject(parent) {}

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

	std::vector<double> data(QString const& deviceId, const uint32_t channelId) noexcept {
		if(dataAcq_.hasId(deviceId))
			return dataAcq_.data(deviceId, channelId);
		return sources_[deviceId]->data(channelId, 500, 0);
	}

	std::vector<double> data(QString const& deviceId, const std::vector<uint32_t>& channelIds) noexcept {
		if (dataAcq_.hasId(deviceId))
			return dataAcq_.data(deviceId, channelIds);
		return sources_[deviceId]->data(channelIds, 500, 0);
	}

	template<typename ScanType>
	void loadDataFromFile(QString const& fileName, QString const& deviceType) noexcept {
		sources_.insert({ fileName, new FileSignalDataSource(fileName, deviceType == "6111" ? DeviceType::_6111 : DeviceType::_6132) });
	}
signals:
	void logMsg(QString const& msg) const;
};