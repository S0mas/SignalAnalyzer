#pragma once
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QString>
#include <vector>
#include <functional>
#include <cstdint>

enum class Frequency {
	_1MHz,
	_2MHz,
	_4MHz,
	_5MHz,
	_10MHz
};

inline const std::vector<Frequency> FREQUENCIES = {Frequency::_1MHz, Frequency::_2MHz, Frequency::_4MHz,Frequency::_5MHz, Frequency::_10MHz };

inline QString toString(const Frequency frequency) {
	switch (frequency) {
	case Frequency::_1MHz:
		return "1Mhz";
	case Frequency::_2MHz:
		return "2Mhz";
	case Frequency::_4MHz:
		return "4Mhz";
	case Frequency::_5MHz:
		return "5Mhz";
	case Frequency::_10MHz:
		return "10Mhz";
	default:
		return "unknown";
	}
}

enum class Operation {
	INIT = 0x00,
	IDLE = 0x01,
	ACQUISITION = 0x11,
	DL_TEST = 0x21
};

inline const std::vector<Operation> OPERATIONS = { Operation::INIT, Operation::IDLE, Operation::ACQUISITION, Operation::DL_TEST };

inline QString toString(const Operation operation) {
	switch (operation) {
	case Operation::INIT:
		return "INIT";
	case Operation::IDLE:
		return "IDLE";
	case Operation::ACQUISITION:
		return "ACQUISITION";
	case Operation::DL_TEST:
		return "DL_TEST";
	default:
		return "unknown";
	}
}

using ChannelId = int;

inline bool isControlButtonHold() noexcept {
	return QApplication::keyboardModifiers() & Qt::ControlModifier;
}

inline bool isShiftButtonHold() noexcept {
	return QApplication::keyboardModifiers() & Qt::ShiftModifier;
}

inline bool isAltButtonHold() noexcept {
	return QApplication::keyboardModifiers() & Qt::AltModifier;
}

inline bool isMouseLeftButtonHold(const QEvent* event) noexcept {
	return static_cast<const QMouseEvent*>(event)->buttons().testFlag(Qt::MouseButton::LeftButton);
}

struct Range {
	std::function<double()> start_;
	std::function<double()> end_;

	Range(const std::function<double()> start, const std::function<double()> end) noexcept : start_(start), end_(end) {}

	/*
		[ this range ]
		  value
	*/
	bool contains(const double value) const noexcept {
		return value >= start_() && value <= end_();
	}

	/*
		[ this range ]
		  [ range ]
	*/
	bool contains(const Range& range) const noexcept {
		return contains(range.start_()) && contains(range.end_());
	}

	/*
		[       range       ]
			[ this range ]
	*/
	bool isContained(const Range& range) const noexcept {
		return range.contains(start_()) && range.contains(end_());
	}

	/*
		[ this range ] value

		value [ this range ]
	*/
	bool separated(const double value) const noexcept {
		return !contains(value);
	}

	/*
		[ this range ] [ range ]
	    
		[ range ] [ this range ]  
	*/
	bool separated(const Range& range) const noexcept {
		return start_() > range.end_() || end_() < range.start_();
	}

	/*
		[ this range ]
		       [ range ]

		[ this range ]
		  [ range ]

			[ this range ]
		[ range ]
	*/
	bool overlaps(const Range& range) const noexcept {
		return !separated(range);
	}

	/*
		value [ this range ]    
	*/
	bool isHigher(const double value) const noexcept {
		return start_() > value;
	}

	/*
		[ range ] [ this range ]
	*/
	bool isHigher(const Range& range) const noexcept {
		return isHigher(range.end_());
	}

	/*
		[ this range ]    value
	*/
	bool isLower(const double value) const noexcept {
		return end_() < value;
	}

	/*
		[ this range ]    [ range ]
	*/
	bool isLower(const Range& range) const noexcept {
		return isLower(range.start_());
	}

	double length() const noexcept {
		return end_() - start_();
	}

	bool isLonger(const Range& range) const noexcept {
		return length() > range.length();
	}
};