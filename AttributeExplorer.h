#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QColumnView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>
#include <QMetaProperty>
#include "PlotItemsContainer.h"

class PropertyItem : public QStandardItem {
	QObject* object_;
public:
	PropertyItem(QObject* propertyHolder) : QStandardItem(propertyHolder->property("nameId").toString()), object_(propertyHolder) {}
	void setData(const QVariant& value, int role = Qt::UserRole + 1) override {
		object_->setProperty("nameId", value);
		QStandardItem::setData(value, role);
		emitDataChanged();
	}

	bool selected() const {
		return object_->property("selected").toBool();
	}

	QStringList propertiesNames() {
		const QMetaObject* metaObject = object_->metaObject();
		QStringList properties;
		for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
			properties << QString::fromLatin1(metaObject->property(i).name());
		return properties;
	}

	QObject* object() {
		return object_;
	}

	//QVariant data(int role = Qt::UserRole + 1) const override {
	//	qDebug() << "Loaded : " << object_->property("nameId").toString();
	//	return object_->property("nameId").toString();
	//}
};

class AttributeExplorer : public QWidget {
	Q_OBJECT
	QColumnView* cview_ = new QColumnView;
	PlotItemsContainer* itemsContainer_;
public:
	AttributeExplorer(PlotItemsContainer& itemsContainer, QWidget* parent = nullptr) : QWidget(parent), itemsContainer_(&itemsContainer) {
		cview_->setModel(new QStandardItemModel);
		connect(cview_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AttributeExplorer::selectionChanged);
		connect(itemsContainer_, &PlotItemsContainer::containerChanged, this, &AttributeExplorer::updateItems);
		auto vlayout = new QVBoxLayout;
		vlayout->addWidget(cview_);
		vlayout->setContentsMargins(0, 0, 0, 0);
		setLayout(vlayout);
	}
public slots:
	void updateItems() {
		auto model = dynamic_cast<QStandardItemModel*>(cview_->model());
		model->clear();
		for (auto const& item : *itemsContainer_) {
			auto propertyItem = new PropertyItem(qobject_cast<QObject*>(item.get()));
			if (propertyItem->selected())
				for (auto& name : propertyItem->propertiesNames()) {
					auto data = propertyItem->object()->property(name.toLocal8Bit().data()).toString();
					qDebug() << data; //TODO 
					propertyItem->appendRow(new QStandardItem(data));
				}
		
			model->appendRow(propertyItem);
		}
	}

	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
		auto model = dynamic_cast<QStandardItemModel*>(cview_->model());
		int index = 0;
		while(index < model->rowCount())
			dynamic_cast<PropertyItem*>(model->itemFromIndex(model->index(index++, 0)))->object()->setProperty("selected", false);
		for(auto const& range : selected)
			for (auto const& index : range.indexes())
				dynamic_cast<PropertyItem*>(model->itemFromIndex(index))->object()->setProperty("selected", true);			
	}
};