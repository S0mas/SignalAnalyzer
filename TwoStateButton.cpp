#include "TwoStateButton.h"
#include <QDebug>

TwoStateButton::TwoStateButton(QString const& actionText1, std::function<void(void)> const& action1, QString const& actionText2, std::function<void(void)> const& action2, QWidget* parent)
	: QPushButton("", parent),
	action1_(action1),
	action2_(action2) {
	timer_->setSingleShot(true);
	connect(this, &QPushButton::clicked, [this]() { action_(); });
	connect(timer_, &QTimer::timeout, this, &TwoStateButton::connectionTimeouted);

	connect(state1_, &QState::entered,
		[this, actionText2]() {
			timer_->stop();
			action_ = [this]() {
				emit actionStarted();
				action2_();
			};
			setText(actionText2);
		}
	);

	connect(state2_, &QState::entered,
		[this, actionText1]() {
			action_ = [this]() {
				emit actionStarted();
				action1_();
			};
			setText(actionText1);
		}
	);

	connect(processing_, &QState::entered,
		[this]() {
			action_ = []() {};
			setText("Processing...");
			timer_->start(5000);
		}
	);

	state2_->addTransition(this, &TwoStateButton::actionStarted, processing_);
	state2_->addTransition(this, &TwoStateButton::connected, state1_);
	processing_->addTransition(this, &TwoStateButton::connected, state1_);
	processing_->addTransition(this, &TwoStateButton::connectionTimeouted, state2_);
	state1_->addTransition(this, &TwoStateButton::disconnected, state2_);

	sm_->addState(state1_);
	sm_->addState(state2_);
	sm_->addState(processing_);
	sm_->setInitialState(state2_);
	sm_->start();
}
