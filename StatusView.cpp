#include "StatusView.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include "Defines.h"

StatusView::StatusView(QWidget * parent) : QWidget(parent) {
	auto layout = new QVBoxLayout;

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(idLabel_);
	hlayout->addWidget(idValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(modeLabel_);
	hlayout->addWidget(modeValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(dataStreamLabel_);
	hlayout->addWidget(dataStreamValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(stateLabel_);
	hlayout->addWidget(stateValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(currentTimeLabel_);
	hlayout->addWidget(currentTimeValue_);
	layout->addLayout(hlayout);

	setLayout(layout);
}

void StatusView::update(Status const& status) noexcept {
	idValue_->setText(toHex(status.id_));
	modeValue_->setText(toString(status.mode_));
	stateValue_->setText(status.state_);
	QString list;
	for (auto stream : status.streams_)
		stream.second ? list.append(QString("[<font color=\"green\">%1</font>]").arg(stream.first)) : list.append(QString("[<font color=\"red\">%1</font>]").arg(stream.first));
	dataStreamValue_->setText(list);
	currentTimeValue_->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy, hh:mm:ss"));
}