#include "ConnectButton.h"
#include <QDebug>

ConnectButton::ConnectButton(std::function<void(void)> const& connectFunction, std::function<void(void)> const& disconnectFunction, QWidget* parent)
	: QPushButton("", parent),
	connectAction_(connectFunction),
	disconnectAction_(disconnectFunction) {
	timer_->setSingleShot(true);
	connect(this, &QPushButton::clicked, [this]() { action_(); });
	connect(timer_, &QTimer::timeout, this, &ConnectButton::connectionTimeouted);

	connect(connected_, &QState::entered,
		[this]() {
			timer_->stop();
			action_ = [this]() {
				emit actionStarted();
				disconnectAction_();
			};
			setText("Disconnect");
		}
	);

	connect(disconnected_, &QState::entered,
		[this]() {
			action_ = [this]() {
				emit actionStarted();
				connectAction_();
			};
			setText("Connect");
		}
	);

	connect(connecting_, &QState::entered,
		[this]() {
			action_ = []() {};
			setText("Connecting...");
			timer_->start(5000);
		}
	);

	disconnected_->addTransition(this, &ConnectButton::actionStarted, connecting_);
	disconnected_->addTransition(this, &ConnectButton::connected, connected_);
	connecting_->addTransition(this, &ConnectButton::connected, connected_);
	connecting_->addTransition(this, &ConnectButton::connectionTimeouted, disconnected_);
	connected_->addTransition(this, &ConnectButton::disconnected, disconnected_);

	sm_->addState(connected_);
	sm_->addState(disconnected_);
	sm_->addState(connecting_);
	sm_->setInitialState(disconnected_);
	sm_->start();
}
