#include "MyPlotMarker.h"
#include "MyPlot.h"

MyPlotMarker::MyPlotMarker(const double x, MyPlot* plot) : QwtPlotMarker(), MyPlotItem(plot) {
	QwtPlotMarker::setTitle("Event 1");
	setLineStyle(QwtPlotMarker::VLine);
	setLinePen(Qt::red, 0, Qt::DashLine);
	QwtPlotMarker::setVisible(true);
	QwtPlotMarker::setItemAttribute(QwtPlotItem::Legend, true);
	setValue(x, 0);
	QwtPlotMarker::attach(plot);
}

void MyPlotMarker::move(const QPointF& destination) noexcept {
	setXValue(round(destination.x()));
}

void MyPlotMarker::setColor(const QColor color) noexcept {
	auto p = linePen();
	p.setColor(color);
	setLinePen(p);
}