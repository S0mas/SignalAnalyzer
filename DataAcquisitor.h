#pragma once
#include "SignalDataSource.h"

struct DeviceInfo {
	DeviceInfo(QString const& deviceAddress, DeviceType const deviceType, ChannelStatuses const& statuses)
		: deviceId_(deviceAddress),
		deviceType_(deviceType),
		statuses_(statuses) {}
	QString deviceId_;
	DeviceType deviceType_;
	ChannelStatuses statuses_;
};

inline bool operator==(DeviceInfo const& devInfo, QString const& ipAddress) {
	return devInfo.deviceId_ == ipAddress;
}

inline bool operator==(QString const& ipAddress, DeviceInfo const& devInfo) {
	return devInfo.deviceId_ == ipAddress;
}

inline uint64_t hostToId(QHostAddress const& address) noexcept {
	return 0;
}

class DataAcquisitor : public QObject {
	Q_OBJECT
	std::map<QString, RealTimeSignalDataSource*> sources_;
	std::atomic<uint32_t> scansToDisplayStep_ = 10;
	uint32_t queuesSize_ = 256;

	PacketReading<SignalPacketHeader, Scan6111>* readingStrategy6111_ = new PacketReading<SignalPacketHeader, Scan6111>;
	PacketReading<SignalPacketHeader, Scan6132>* readingStrategy6132_ = new PacketReading<SignalPacketHeader, Scan6132>;
	DataCollectorServer* dataCollector6111_ = new DataCollectorServer(forwardingPort6111, readingStrategy6111_, this);
	DataCollectorServer* dataCollector6132_ = new DataCollectorServer(forwardingPort6132, readingStrategy6132_, this);
	std::map<QHostAddress, DataCollectorClient*> clients_;
public:
	DataAcquisitor(QObject* parent = nullptr) : QObject(parent) {
		readingStrategy6111_->setDataReadingCallback([this](SignalPacketHeader const& header, SignalPacketData<Scan6111> const& data) { handleData(header, data); });
		readingStrategy6132_->setDataReadingCallback([this](SignalPacketHeader const& header, SignalPacketData<Scan6132> const& data) { handleData(header, data); });

		connect(dataCollector6111_, &DataCollectorServer::newClientConnected, [this]() {emit logMsg("New 6111 client connected!"); });
		connect(dataCollector6111_, &DataCollectorServer::clientDisconnected, [this]() {emit logMsg("6111 client disconnected!"); });
		connect(dataCollector6111_, &DataCollectorServer::logMsg, [this](QString const& msg) {emit logMsg("6111 " + msg); });

		connect(dataCollector6132_, &DataCollectorServer::newClientConnected, [this]() {emit logMsg("New 6132 client connected!"); });
		connect(dataCollector6132_, &DataCollectorServer::clientDisconnected, [this]() {emit logMsg("6132 client disconnected!"); });
		connect(dataCollector6132_, &DataCollectorServer::logMsg, [this](QString const& msg) {emit logMsg("6132 " + msg); });
	}

	template<typename ScanType>
	void handleData(SignalPacketHeader const& header, SignalPacketData<ScanType> const& data) {
		if (sources_.find(header.deviceAddress_.toString()) == sources_.end()) {
			//emit sourceCreated(new RealTimeSignalDataSource(header.deviceAddress_.toString(), header.deviceType_));
			sources_.insert({ header.deviceAddress_.toString() , new RealTimeSignalDataSource(header.deviceAddress_.toString(), header.deviceType_) });
		}
		sources_[header.deviceAddress_.toString()]->enqueueData(header, data);
	}

	QStringList devIds() const noexcept {
		QStringList names;
		for (auto const& dev : sources_)
			names.push_back(dev.first);
		return names;
	}

	bool hasId(QString const& devId) const noexcept {
		return sources_.find(devId) != sources_.end();
	}

	ChannelStatuses& statuses(QString const& devId) noexcept {
		return sources_.at(devId)->statuses();
	}

	DeviceType deviceType(QString const& devId) const noexcept {
		return sources_.at(devId)->deviceType();
	}

	void setScansToDisplayStep(uint32_t const step) noexcept {
		scansToDisplayStep_ = step;
	}

	uint32_t scansToDisplayStep() const noexcept {
		return scansToDisplayStep_;
	}

	uint32_t queuesSize() const noexcept {
		return queuesSize_;
	}

	void setQueuesSize(uint32_t const size) noexcept {
		queuesSize_ = size;
		for (auto& id_source_pair : sources_)
			id_source_pair.second->setQueuesSize(size);
	}

	auto data(QString const& deviceId, const uint32_t channelId) noexcept {
		return sources_[deviceId]->data(channelId, 0, 0);
	}

	auto data(QString const& deviceId, const std::vector<uint32_t>& channelIds) noexcept {
		return sources_[deviceId]->data(channelIds, 0, 0);
	}
signals:
	void logMsg(QString const& msg) const;
	void sourceCreated(SignalDataSource* source) const;
};