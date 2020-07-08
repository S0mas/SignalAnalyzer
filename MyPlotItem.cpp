#pragma once
#include "MyPlotItem.h"
#include "MyPlotIntervalCurve.h"
#include "MyPlot.h"
#include "qwt_scale_widget.h"

MyPlotItem::MyPlotItem(MyPlot* plot, MyPlotItem* parent) : root_(parent ? parent : this) {
	plot_ = plot;
	connect(this, &MyPlotItem::selectedChanged, this, &MyPlotItem::updateColorAfterSelectionChanged);
}

bool MyPlotItem::contains(const QPointF& point) const noexcept {
	return boundingRect().contains(point);
}
bool MyPlotItem::intersects(const QRectF& rect) const noexcept {
	return boundingRect().intersects(rect);
}

bool MyPlotItem::isPositionedExclusive() const noexcept {
	return isPositionedExclusive_;
}

bool MyPlotItem::isSelected() const noexcept {
	return selected_;
}

bool MyPlotItem::trySelect(const QPointF& point) noexcept {
	if (contains(point)) {
		setProperty("selected", true);
		return true;
	}
	return false;
}

bool MyPlotItem::trySelect(const QRectF& rect) noexcept {
	if (intersects(rect)) {
		setProperty("selected", true);
		return true;
	}
	return false;
}

void MyPlotItem::updateColorAfterSelectionChanged(bool const selected) {
	selected ? setColor(Qt::GlobalColor::cyan) : setColor(mainColor_);
}