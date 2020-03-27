#pragma once
#include <QString>



class Test {
	TestType testType_;
	int count_;
	int errors_;
public:
	Test() : testType_(TestType::UNKNOWN), count_(0), errors_(0) {}
	Test(TestType const& type) : testType_(type), count_(0), errors_(0) {}

	void reset() noexcept {
		count_ = 0;
		errors_ = 0;
	}

	void set(int const count, int const errors) noexcept {
		count_ = count;
		errors_ = errors;
	}

	int count() const noexcept {
		return count_;
	}

	int errors() const noexcept {
		return errors_;
	}

	QString name() const noexcept {
		return toString(testType_);
	}

	TestType type() const noexcept {
		return testType_;
	}
};