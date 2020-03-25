#pragma once
#include <QCheckBox>
#include <QEvent>
#include <QSignalTransition>
#include <QStateMachine>

class CheckedTransition : public QSignalTransition {
public:
	CheckedTransition(QCheckBox* checkBox)
		: QSignalTransition(checkBox, &QCheckBox::stateChanged) {}
	virtual ~CheckedTransition() = default;
protected:
	bool eventTest(QEvent* e) override {
		return QSignalTransition::eventTest(e) ? static_cast<QStateMachine::SignalEvent*>(e)->arguments().at(0).toInt() == Qt::Checked : false;
	}
};

class UncheckedTransition : public QSignalTransition {
public:
	UncheckedTransition(QCheckBox* checkBox)
		: QSignalTransition(checkBox, &QCheckBox::stateChanged) {}
	virtual ~UncheckedTransition() = default;
protected:
	bool eventTest(QEvent* e) override {
		return QSignalTransition::eventTest(e) ? static_cast<QStateMachine::SignalEvent*>(e)->arguments().at(0).toInt() == Qt::Unchecked : false;
	}
};