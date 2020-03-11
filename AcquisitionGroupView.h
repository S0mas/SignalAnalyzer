#pragma once
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

class AcquisitionGroupView : public QGroupBox {
	Q_OBJECT
public:
	AcquisitionGroupView(QWidget* parent = nullptr) : QGroupBox("Acquisition Controller", parent) {
		auto startAcqButton = new QPushButton("Start Acq", this);
		connect(startAcqButton, &QPushButton::clicked, this, &AcquisitionGroupView::startAcquisition);

		auto stopAcqButton = new QPushButton("Stop Acq", this);
		connect(stopAcqButton, &QPushButton::clicked, this, &AcquisitionGroupView::stopAcquisition);
		stopAcqButton->setDisabled(true);

		connect(startAcqButton, &QPushButton::clicked, [startAcqButton, stopAcqButton]() {startAcqButton->setDisabled(true); stopAcqButton->setEnabled(true); });
		connect(stopAcqButton, &QPushButton::clicked, [startAcqButton, stopAcqButton]() {startAcqButton->setEnabled(true); stopAcqButton->setDisabled(true); });
		connect(this, &AcquisitionGroupView::acquisitionStarted, [startAcqButton, stopAcqButton]() {startAcqButton->setDisabled(true); stopAcqButton->setEnabled(true); });
		connect(this, &AcquisitionGroupView::acquisitionStopped, [startAcqButton, stopAcqButton]() {startAcqButton->setEnabled(true); stopAcqButton->setDisabled(true); });

		auto layout = new QGridLayout(this);
		layout->addWidget(startAcqButton);
		layout->addWidget(stopAcqButton);
		setLayout(layout);
	}
signals:
	//output
	void startAcquisition() const;
	void stopAcquisition() const;

	//input
	void acquisitionStarted() const;
	void acquisitionStopped() const;
};