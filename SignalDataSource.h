#pragma once
#include "DataGenerator.h"
#include "SignalDataPacket.h"
#include "Defines.h"
#include <QContiguousCache>
#include "gui/ChannelStatus.h"
#include "DataCollectorServer.h"
#include "PacketReading.h"
#include <mutex>
#include <QString>
#include <atomic>
#include <vector>

template<typename T>
inline std::vector<T> take(uint32_t const maximumElementsToTake, QContiguousCache<T>& queue) {
	std::vector<T> taken;
	int32_t toTake = maximumElementsToTake > queue.size() ? queue.size() : maximumElementsToTake;
	taken.reserve(toTake);
	while (toTake-- > 0)
		taken.push_back(queue.takeFirst());
	return taken;
}

class SignalDataSource : public QObject {
	Q_OBJECT
protected:
	QString deviceName_;
	DeviceType deviceType_;
	ChannelStatuses statuses_;
public:
	SignalDataSource(QString const& deviceName, DeviceType const& deviceType, QObject* parent = nullptr) : QObject(parent), deviceName_(deviceName), deviceType_(deviceType), statuses_(deviceType == DeviceType::_6111 ? 256 : 32) {
		statuses_.enableAll();
	}
	virtual std::pair<std::vector<double>, std::vector<Timestamp6991>> data(uint32_t const channelId, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept = 0;
	virtual std::pair<std::vector<double>, std::vector<Timestamp6991>> data(std::vector<uint32_t> const& channelIds, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept = 0;
	ChannelStatuses& statuses() noexcept {
		return statuses_;
	}

	DeviceType deviceType() const noexcept {
		return deviceType_;
	}

	virtual bool isRealTimeSource() const noexcept = 0;
signals:
	void logMsg(QString const& msg) const;
};

class RealTimeSignalDataSource : public SignalDataSource {
	Q_OBJECT
	std::vector<QContiguousCache<double>> data_;
	QContiguousCache<Timestamp6991> timestampsQueue_;
	mutable std::mutex m_;
	uint32_t queuesSize_;
	uint32_t scansToDisplayStep_;
public:
	RealTimeSignalDataSource(QString fileName, DeviceType const type, uint32_t const queueSize = 256, uint32_t const scansToDisplayStep = 10, QObject* parent = nullptr) : SignalDataSource(fileName, type, parent), queuesSize_(queueSize), scansToDisplayStep_(scansToDisplayStep) {
		for (auto const& channelId : statuses_.allEnabled())
			data_.push_back(QContiguousCache<double>(queuesSize_));
	}

	std::pair<std::vector<double>, std::vector<Timestamp6991>> data(uint32_t const channelId, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept override {
		std::lock_guard lock(m_);
		auto copy = data_[channelId - 1];
		auto copyTs = timestampsQueue_;
		return {take(copy.size(), copy), take(copyTs.size(), copyTs)};
	}

	std::pair<std::vector<double>, std::vector<Timestamp6991>> data(std::vector<uint32_t> const& channelIds, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept override {
		std::vector<std::vector<double>> vecs;
		vecs.resize(channelIds.size());
		uint32_t samplesCount = std::numeric_limits<uint32_t>::max();
		QContiguousCache<Timestamp6991> copyTs;
		{
			std::lock_guard lock(m_);
			copyTs = timestampsQueue_;
			for (int i = 0; i < channelIds.size(); ++i) {
				auto copy = data_[channelIds[i] - 1];
				if (copy.size() < samplesCount)
					samplesCount = copy.size();
				vecs[i] = take(copy.size(), copy);
			}
		}
		std::vector<double> result;
		result.resize(samplesCount);
		for (int sampleNo = 0; sampleNo < samplesCount; ++sampleNo)
			for (int bitNo = 0; bitNo < vecs.size(); ++bitNo)
				result[sampleNo] += vecs[bitNo][sampleNo] ? 1 << bitNo : 0;
		return { result, take(samplesCount, copyTs) };
	}

	void enqueueData(SignalPacketHeader const& header, SignalPacketData<Scan6111> const& data) {
		for (int scanId = 0; scanId < data.scans_.size(); scanId += scansToDisplayStep_) {
			uint32_t channelId = 0;
			if (data.scans_[scanId].ignoreTimestamp)
				timestampsQueue_.append({ 0, 0 });
			else
				timestampsQueue_.append(data.scans_[scanId].ts_);
			for (auto& samplePack : data.scans_[scanId].samples_) {
				for (auto& sample : samplePack.samples())
					data_[channelId++].append(sample ? 1 : 0);
			}
		}
	}

	void enqueueData(SignalPacketHeader const& header, SignalPacketData<Scan6132> const& data) {
		for (int scanId = 0; scanId < data.scans_.size(); scanId += scansToDisplayStep_) {
			uint32_t channelId = 0;
			if (data.scans_[scanId].ignoreTimestamp)
				timestampsQueue_.append({ 0, 0 });
			else
				timestampsQueue_.append(data.scans_[scanId].ts_);
			for (auto& sample : data.scans_[scanId].samples_)
				data_[channelId++].append(sample);
		}
	}

	void setQueuesSize(uint32_t const size) noexcept {
		queuesSize_ = size;
		for (auto& buffer : data_)
			buffer.setCapacity(size);
	}

	void setScansToDisplayStep(uint32_t const step) noexcept {
		scansToDisplayStep_ = step;
	}

	bool isRealTimeSource() const noexcept override {
		return true;
	}
};

class FileSignalDataSource : public SignalDataSource {
	Q_OBJECT
	std::vector<QList<double>> data_;
	QList<Timestamp6991> timestamps_;
public:
	void readScan(Scan6111 const& scan) noexcept {
		if (scan.ignoreTimestamp)
			timestamps_.append({ 0, 0 });
		else
			timestamps_.append(scan.ts_);
		for (int i = 0; i < scan.samples_.size(); ++i)
			for (int j = 0; j < scan.samples_[i].samples().size(); ++j)
				data_[i*8 + j].push_back(scan.samples_[i].samples()[j] ? 1 : 0);
	}

	void readScan(Scan6132 const& scan) noexcept {
		if (scan.ignoreTimestamp)
			timestamps_.append({ 0, 0 });
		else
			timestamps_.append(scan.ts_);
		for (int i = 0; i < scan.samples_.size(); ++i)
			data_[i].push_back(scan.samples_[i]);
	}

	template<typename ScanType>
	void readFile(QString const& fileName) {
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return;
		QTextStream in(&file);
		while (!in.atEnd()) {
			ScanType scan;
			in >> scan;
			readScan(scan);
		}
	}
	FileSignalDataSource(QString fileName, DeviceType const type,  QObject* parent = nullptr) : SignalDataSource(fileName, type, parent) {
		for (auto const& channelId : statuses_.allEnabled()) {
			QList<double> list;
			list.reserve(10000);
			data_.push_back(list);
		}
		
		if(type == DeviceType::_6111)
			readFile<Scan6111>(fileName);
		else
			readFile<Scan6132>(fileName);		
	}

	std::vector<double> dataWithoutTimestamps(uint32_t const channelId, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept {
		auto const& signalData = data_[channelId - 1];
		if (startSampleId >= signalData.size())
			return {};
		uint32_t count = (numberOfSamples + startSampleId > signalData.size()) || numberOfSamples == 0 ? signalData.size() - startSampleId : numberOfSamples;
		return std::vector<double>(signalData.begin() + startSampleId, signalData.begin() + count);
	}

	std::pair<std::vector<double>, std::vector<Timestamp6991>> data(uint32_t const channelId, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept override {
		auto const& signalData = data_[channelId - 1];
		if(startSampleId >= signalData.size())
			return {};
		uint32_t count = (numberOfSamples + startSampleId > signalData.size()) || numberOfSamples == 0 ? signalData.size() - startSampleId : numberOfSamples;
		return { std::vector<double>(signalData.begin() + startSampleId, signalData.begin() + count), std::vector<Timestamp6991>(timestamps_.begin() + startSampleId, timestamps_.begin() + count) };
	}

	std::pair<std::vector<double>, std::vector<Timestamp6991>> data(std::vector<uint32_t> const& channelIds, uint32_t const numberOfSamples, uint32_t const startSampleId = 0) noexcept override {
		auto result = data(channelIds[0], numberOfSamples, startSampleId);
		
		for (int i = 1; i < channelIds.size(); ++i) {
			auto const& next = dataWithoutTimestamps(channelIds[i], numberOfSamples, startSampleId);
			for (int j = 0; j << next.size(); ++j)
				result.first[j] += next[j] * (1 << i);
		}
		return result;
	}

	QString devId() const noexcept {
		return deviceName_;
	}

	ChannelStatuses& statuses(QString const& devId) noexcept {
		return statuses_;
	}

	bool isRealTimeSource() const noexcept override {
		return false;
	}
signals:
	void logMsg(QString const& msg) const;
};
