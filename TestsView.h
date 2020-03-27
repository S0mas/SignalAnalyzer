#pragma once
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QStateMachine>
#include <QTime>
#include <QTimer>
#include "AbstractDevice.h"
#include <map>
#include <set>
#include "Defines.h"
#include "Test.h"
#include "TwoStateButton.h"

class TestsResultView : public QGroupBox {
	Q_OBJECT
	struct ResultView {
		TestType type_;
		QLabel* testName_;
		QLabel* count_ = new QLabel("0");
		QLabel* errors_ = new QLabel("0");
		ResultView(TestType const& type) noexcept;
		void setModel(Result const& result) noexcept;
		Result model() const noexcept;
	};
	std::vector<ResultView*> resultViews_;
	QStringList resultLabels = { "Count", "Errors" };
	void addResult(TestType const type, QGridLayout * layout, int const row) noexcept;
public:
	TestsResultView(QWidget* parent = nullptr);
	TestsResultModel model() const noexcept;
	void resetModel() const noexcept;
public slots:
	void setModel(TestsResultModel const& model) const noexcept;
};

class TestSelectionView : public QGroupBox {
	Q_OBJECT
	struct TestSelectView {
		TestType type_;
		QCheckBox* checkBox_;
		TestSelectView(TestType const type) : type_(type), checkBox_(new QCheckBox(toString(type))) {}
	};
	std::vector<TestSelectView*> testSelectViews_;
public:
	TestSelectionView(QWidget* parent = nullptr);
	void setModel(TestsSelectionModel const& model) const noexcept;
	TestsSelectionModel model() const noexcept;
};

class TestsView : public QGroupBox {
	Q_OBJECT
	TestsResultView* resultView_ = new TestsResultView;
	TestSelectionView* selectionView_ = new TestSelectionView;
	QLabel* testElapsedTimeLabel_ = new QLabel("Time elapsed:");
	TwoStateButton* startStopTestsButton_;
	QStateMachine sm_;
	QTime time_;
private:
	void createConnections(AbstractDevice* device) noexcept;
	void initializeStateMachine(AbstractDevice* device) noexcept;
	void updateTime() const noexcept;
public:
	TestsView(AbstractDevice* device, QWidget* parent = nullptr);
};