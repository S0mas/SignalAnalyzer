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
#include "Test.h"

class AutoTestController : public QGroupBox {
	Q_OBJECT
	std::map<TestType, Test> tests_;
	std::set<TestType> selectedTests_;
	QStateMachine sm_;
	QTime time_;
	QStringList resultLabels = { "Count", "Errors" };
signals:
	void refreshCounters() const;
	void refreshTime() const;
private:
	void initializeStateMachine(AbstractDevice* device, QWidget* testsGroup, QWidget* startButton, QWidget* stopButton) noexcept;
	void clearCounters() noexcept;	
	void addTestResultRow(TestType const type, QGridLayout* layout, int const row) const noexcept;
	QWidget* resultsGroup() const noexcept;
	QWidget* testsGroup() noexcept;
public:
	AutoTestController(AbstractDevice* device, QWidget* parent = nullptr);
public slots:
	void handleCounters(std::vector<Test> const& tests) noexcept;
};