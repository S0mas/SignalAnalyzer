#pragma once
#include <QObject>
#include <QDebug>
#include "Defines.h"

class Controller : public QObject {
	Q_OBJECT
public slots:
	void startAcquisition() const {
		qDebug() << __func__;
	}

	void stopAcquisition() const {
		qDebug() << __func__;
	}

	void setDlLinkClockFrequency(const Frequency frequency) const {
		qDebug() << QString("%1 frequency: %2").arg(__func__, toString(frequency));
	}

	void toogleChannel(const unsigned int channelId, const bool state) const {
		qDebug() << QString("%1 channelId: %2 to state: %3").arg(__func__).arg(channelId + 1).arg(state);
	}

	void toogleChannelsGroup(const unsigned int groupId, const bool state) const {
		qDebug() << QString("%1 groupId: %2 to state: %3").arg(__func__).arg(groupId + 1).arg(state);
	}

	void toogleAllChannels(const bool state) const {
		qDebug() << QString("%1 to state: %2").arg(__func__).arg(state);
	}

	void setChannel() const {

	}

	void slot6() const {

	}

	void slot7() const {

	}

signals:
	//output
	void acquisitionStarted() const;
	void acquisitionStopped() const;
	
	//input
};