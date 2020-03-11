#pragma once
#include <QList>
#include <qwt_picker_machine.h>
#include <qwt_event_pattern.h>

class MyPickerDragRectMachine : public QwtPickerMachine {
public:
	MyPickerDragRectMachine() : QwtPickerMachine(RectSelection) {
	
	}

	//! Transition
	QList<QwtPickerMachine::Command> transition(const QwtEventPattern& eventPattern, const QEvent* event) override {
		QList<QwtPickerMachine::Command> cmdList;

		switch (event->type()) {
		case QEvent::MouseButtonPress:
		{
			if (eventPattern.mouseMatch(QwtEventPattern::MouseSelect1,
				static_cast<const QMouseEvent*>(event)) || eventPattern.mouseMatch(QwtEventPattern::MouseSelect2,
					static_cast<const QMouseEvent*>(event))) {
				if (state() == 0) {
					cmdList += Begin;
					cmdList += Append;
					cmdList += Append;
					setState(2);
				}
			}
			break;
		}
		case QEvent::MouseMove:
		case QEvent::Wheel:
		{
			if (state() != 0)
				cmdList += Move;
			break;
		}
		case QEvent::MouseButtonRelease:
		{
			if (state() == 2) {
				cmdList += End;
				setState(0);
			}
			break;
		}
		case QEvent::KeyPress:
		{
			if (eventPattern.keyMatch(QwtEventPattern::KeySelect1,
				static_cast<const QKeyEvent*> (event))) {
				if (state() == 0) {
					cmdList += Begin;
					cmdList += Append;
					cmdList += Append;
					setState(2);
				}
				else {
					cmdList += End;
					setState(0);
				}
			}
			break;
		}
		default:
			break;
		}

		return cmdList;
	}
};
