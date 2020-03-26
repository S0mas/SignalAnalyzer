#include "AutoTestController.h"

void AutoTestController::initializeStateMachine(AbstractDevice * device, QWidget * testsGroup, QWidget * startButton, QWidget * stopButton) noexcept {
	auto idle = new QState();
	auto running = new QState();

	idle->addTransition(device, &AbstractDevice::testsStarted, running);
	running->addTransition(device, &AbstractDevice::testsStopped, idle);

	auto timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &AutoTestController::refreshTime);
	sm_.addState(idle);
	sm_.addState(running);

	connect(idle, &QState::entered,
		[this, testsGroup, startButton, stopButton, timer]() {
			testsGroup->setEnabled(true);
			startButton->setEnabled(true);
			stopButton->setEnabled(false);
			timer->stop();
		}
	);
	connect(running, &QState::entered,
		[this, testsGroup, startButton, stopButton, timer]() {
			testsGroup->setEnabled(false);
			startButton->setEnabled(false);
			stopButton->setEnabled(true);
			clearCounters();
			time_.restart();
			timer->start(1000);
			emit refreshTime();
		}
	);
	sm_.setInitialState(idle);
	sm_.start();
}

void AutoTestController::clearCounters() noexcept {
	for (auto& test : tests_)
		test.second.reset();
	emit refreshCounters();
}

void AutoTestController::addTestResultRow(TestType const type, QGridLayout * layout, int const row) const noexcept {
	auto widget = new QWidget;

	auto label = new QLabel(toString(type));
	layout->addWidget(label, row, 0, Qt::AlignLeft | Qt::AlignVCenter);

	auto count = new QLabel("0");
	layout->addWidget(count, row, 1, Qt::AlignCenter);

	auto errors = new QLabel("0");
	layout->addWidget(errors, row, 2, Qt::AlignCenter);

	connect(this, &AutoTestController::refreshCounters, [count, type, this]() {count->setText(QString::number(tests_.at(type).count())); });
	connect(this, &AutoTestController::refreshCounters, [errors, type, this]() {errors->setText(QString::number(tests_.at(type).errors())); });
}

QWidget * AutoTestController::resultsGroup() const noexcept {
	auto layout = new QGridLayout;
	for (auto resultLabel : resultLabels)
		layout->addWidget(new QLabel(resultLabel), 0, layout->columnCount(), Qt::AlignCenter);
	for (auto test : tests_)
		addTestResultRow(test.first, layout, layout->rowCount());

	auto widget = new QGroupBox("Results");
	widget->setLayout(layout);
	return widget;
}

QWidget * AutoTestController::testsGroup() noexcept {
	auto layout = new QVBoxLayout;
	for (auto test : tests_) {
		auto testCheckBox = new QCheckBox(test.second.name());
		layout->addWidget(testCheckBox);
		connect(testCheckBox, &QCheckBox::stateChanged,
			[this, type = test.first](auto state) {
			if (state == Qt::Checked)
				selectedTests_.insert(type);
			else
				selectedTests_.erase(type);
		});
	}
	auto widget = new QGroupBox("Tests");
	widget->setLayout(layout);
	return widget;
}

AutoTestController::AutoTestController(AbstractDevice* device, QWidget* parent) : QGroupBox("Auto Test Controller", parent) {
	for (auto testType : testTypes)
		tests_[testType] = Test(testType);
	auto testsSelector = testsGroup();
	auto testElapsedTimeLabel = new QLabel("Time elapsed:");
	auto executeButton = new QPushButton("Execute");
	auto stopButton = new QPushButton("Stop");

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(testsSelector);
	hlayout->addWidget(resultsGroup());

	auto layout = new QVBoxLayout;
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(stopButton);
	hlayout->addWidget(executeButton, 1, Qt::AlignRight);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(testElapsedTimeLabel);
	layout->addLayout(hlayout);
	setLayout(layout);

	connect(executeButton, &QPushButton::clicked,
		[device, this]() {
			std::vector<TestType> types;
			for (auto type : selectedTests_)
				types.push_back(type);
			device->handleStartTestsReq(types);
		});
	connect(stopButton, &QPushButton::clicked, device, &AbstractDevice::handleStopTestsReq);
	connect(this, &AutoTestController::refreshTime,
		[testElapsedTimeLabel, this]() {
			auto msecs = time_.elapsed();
			auto seconds = (msecs / 1000) % 60;
			auto minutes = (msecs / 60000) % 60;
			testElapsedTimeLabel->setText(QString("Time elapsed: %1m %2s").arg(minutes).arg(seconds));
			testElapsedTimeLabel->width();
		});
	connect(device, &AbstractDevice::sendCounters, this, &AutoTestController::handleCounters);
	initializeStateMachine(device, testsSelector, executeButton, stopButton);
}

void AutoTestController::handleCounters(std::vector<Test> const& tests) noexcept {
	for (auto const& test : tests)
		tests_[test.type()] = test;
	emit refreshCounters();
}