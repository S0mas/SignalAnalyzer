#pragma once
#include "MyPlotItem.h"
#include "MyPlotIntervalCurve.h"
#include "MyPlot.h"
#include "qwt_scale_widget.h"

MyPlotItem::MyPlotItem(MyPlot* plot, MyPlotItem* parent) : root_(parent ? parent : this) {
	plot_ = plot;
	//auto canvas = dynamic_cast<QwtPlotCanvas*>(plot->canvas());
	//canvas->installEventFilter(this);
	//canvas->setFocusPolicy(Qt::StrongFocus);
	//canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
}

bool MyPlotItem::contains(const QPointF& point) const noexcept {
	return boundingRect().contains(point);
}
bool MyPlotItem::intersects(const QRectF& rect) const noexcept {
	return boundingRect().intersects(rect);
}

void MyPlotItem::deselect() noexcept {
	selected_ = false;
	setColor(mainColor_);
}

bool MyPlotItem::isPositionedExclusive() const noexcept {
	return isPositionedExclusive_;
}

bool MyPlotItem::isSelected() const noexcept {
	return selected_;
}

void MyPlotItem::select() noexcept {
	selected_ = true;
	setColor(Qt::GlobalColor::cyan);
}

bool MyPlotItem::trySelect(const QPointF& point) noexcept {
	if (contains(point)) {
		select();
		return true;
	}
	return false;
}

bool MyPlotItem::trySelect(const QRectF& rect) noexcept {
	if (intersects(rect)) {
		select();
		return true;
	}
	return false;
}
