#pragma once
#include <QGroupBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>

struct AcquisitionStartModeModel;

class AcquisitionStartModeView : public QGroupBox {
	Q_OBJECT
	QComboBox* modeComboBox_ = new QComboBox;
	QDateTimeEdit* startDateTime_ = new QDateTimeEdit;
	QPushButton* nowButton_ = new QPushButton("Now");

	void createConnections() noexcept;
public:
	AcquisitionStartModeView(QWidget* parent = nullptr);
	AcquisitionStartModeModel model() const noexcept;
	void setModel(AcquisitionStartModeModel const& model) noexcept;
};

