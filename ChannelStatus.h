#pragma once
#include <map>
#include <QPushButton>
#include <QMutex>
#include <QDebug>
#include "Defines.h"

struct ChannelStatus {
	const ChannelId id_;
	bool enabled_;
	ChannelStatus(const int id) noexcept : id_(id), enabled_(false) {}
};

class ChannelStatuses {
	std::map<ChannelId, ChannelStatus> statuses_;
public:
	ChannelStatuses(const int channelsNo) noexcept {
		for (ChannelId i = 1; i <= channelsNo; ++i)
			statuses_.insert({ i, ChannelStatus(i) });
	}

	void disable(const ChannelId id) noexcept {
		statuses_.at(id).enabled_ = false;
	}

	void disableAll() noexcept {
		for(auto& status : statuses_)
			status.second.enabled_ = false;
	}

	void enable(const ChannelId id) noexcept {
		statuses_.at(id).enabled_ = true;
	}

	void enableAll() noexcept {
		for (auto& status : statuses_)
			status.second.enabled_ = true;
	}

	void set(const ChannelId id, const bool state) noexcept {
		statuses_.at(id).enabled_ = state;
	}

	bool status(const ChannelId id) const noexcept {
		return statuses_.at(id).enabled_;
	}

	int size() const noexcept {
		return statuses_.size();
	}

	QVector<bool> all() const noexcept {
		QVector<bool> res;
		res.reserve(size());
		for (auto const& status : statuses_)
			res.push_back(status.second.enabled_);
		return res;
	}

	std::vector<ChannelId> allEnabled() const noexcept {
		std::vector<ChannelId> res;
		res.reserve(size());
		for (auto const& status : statuses_)
			if(status.second.enabled_)
				res.push_back(status.first);
		return res;
	}
};

class ChannelSelectionButton : public QPushButton {
	Q_OBJECT
	ChannelId id_;
public:
	ChannelSelectionButton(const ChannelId id, QWidget* parent = nullptr) : QPushButton(QString::number(id), parent), id_(id){
		setFixedWidth(25);
		setCheckable(true);
	}

	ChannelId id() const noexcept {
		return id_;
	}
};