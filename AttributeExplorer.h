#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QColumnView>
#include <QStandardItemModel>
#include <QStandardItem>

class AttributeExplorer : public QWidget {
	Q_OBJECT
	QColumnView* propertiesColumnView() {
		QColumnView* cview = new QColumnView;

		/* Create the data model */
		auto model = new QStandardItemModel;

		for (int groupnum = 0; groupnum < 3; ++groupnum) {
			/* Create the phone groups as QStandardItems */
			QStandardItem* group = new QStandardItem(QString("Group %1").arg(groupnum));

			/* Append to each group 5 person as children */
			for (int personnum = 0; personnum < 5; ++personnum) {
				QStandardItem* child = new QStandardItem(QString("Person %1 (group %2)").arg(personnum).arg(groupnum));
				/* the appendRow function appends the child as new row */
				group->appendRow(child);
			}
			/* append group as new row to the model. model takes the ownership of the item */
			model->appendRow(group);
		}

		cview->setModel(model);

		return cview;
	}
public:
	AttributeExplorer(QWidget* parent = nullptr) : QWidget(parent) {
		auto vlayout = new QVBoxLayout;
		vlayout->addWidget(propertiesColumnView());
		vlayout->setContentsMargins(0, 0, 0, 0);
		setLayout(vlayout);
	}
};