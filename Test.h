#pragma once
#include <QString>

enum class TestType {
	CL0,
	CL1,
	DL0,
	DL1,
	FIFO,
	UNKNOWN
};

inline std::vector<TestType> testTypes = { TestType::CL0 , TestType::CL1, TestType::DL0, TestType::DL1, TestType::FIFO };
inline QString toString(TestType const test) noexcept {
	switch (test) {
	case TestType::CL0:
		return "CL0";
	case TestType::CL1:
		return "CL1";
	case TestType::DL0:
		return "DL0";
	case TestType::DL1:
		return "DL1";
	case TestType::FIFO:
		return "FIFO";
	default:
		return "UNKNOWN";
	}
}

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