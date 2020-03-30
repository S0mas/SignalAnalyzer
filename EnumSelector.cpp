#include "EnumSelector.h"
#include "SmartEnum.h"

#include <QHBoxLayout>
#include <QLabel>

void EnumSelector::createConnections() const noexcept {
	QObject::connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this](int const index) {
			lineEdit_->setText(comboBox_->toHex());
			comboBox_->isCustomSelected() ? lineEdit_->setEnabled(true) : lineEdit_->setDisabled(true);
		}
	);
	emit comboBox_->currentIndexChanged(comboBox_->currentIndex());
}

EnumSelector::EnumSelector(std::unique_ptr<SmartEnum>&& smartEnum, QString const& name, QWidget * parent) 
	: QWidget(parent), label_(new QLabel(name)), comboBox_(new EnumComboBox(smartEnum)) {
	label_->setMinimumWidth(50);
	label_->setMaximumWidth(50);

	lineEdit_->setMaximumWidth(50);
	lineEdit_->setDisabled(true);
	lineEdit_->setInputMask(comboBox_->inputMask());

	auto layout = new QHBoxLayout;
	layout->addWidget(label_);
	layout->addWidget(comboBox_);
	layout->addWidget(lineEdit_);

	setLayout(layout);
	createConnections();
}

unsigned int EnumSelector::value() const noexcept {
	return lineEdit_->text().toUInt(nullptr, 16);
}
