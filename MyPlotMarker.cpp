#include "MyPlotMarker.h"
#include "MyPlot.h"

MyPlotMarker::MyPlotMarker(const double x, MyPlot* plot, MyPlotItem* parent) : QwtPlotMarker(), MyPlotItem(plot, parent) {
	QwtPlotMarker::setTitle("Event 1");
	setLineStyle(QwtPlotMarker::VLine);
	setLinePen(Qt::red, 0, Qt::DashLine);
	QwtPlotMarker::setVisible(true);
	QwtPlotMarker::setItemAttribute(QwtPlotItem::Legend, true);
	setValue(x, 0);
	QwtPlotMarker::attach(plot);
}

void MyPlotMarker::move(const double distanceX, const double distanceY) noexcept {
	setXValue(xValue() + round(distanceX));
}

void MyPlotMarker::setColor(const QColor color) noexcept {
	auto p = linePen();
	p.setColor(color);
	setLinePen(p);
}