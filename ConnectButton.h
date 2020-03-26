#pragma once
#include <QPushButton>
#include <QStateMachine>
#include <QTimer>

#include <functional>

class ConnectButton : public QPushButton {
	Q_OBJECT
	QStateMachine* sm_ = new QStateMachine(this);
	std::function<void(void)> action_;
	std::function<void(void)> connectAction_;
	std::function<void(void)> disconnectAction_;
	QState* connected_ = new QState;
	QState* disconnected_ = new QState;
	QState* connecting_ = new QState;
	QTimer* timer_ = new QTimer;
signals:
	void actionStarted() const;
	void connectionTimeouted() const;
public:
	ConnectButton(std::function<void(void)> const& connectFunction, std::function<void(void)> const& disconnectFunction, QWidget* parent = nullptr);
signals:
	void connected() const;
	void disconnected() const;
};

