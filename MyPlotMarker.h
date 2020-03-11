#pragma once

#include <qwt_plot_marker.h>
#include "MyPlotItem.h"
#include <QDebug>

class MyPlotMarker : public QwtPlotMarker, public MyPlotItem {
public:
	MyPlotMarker(const double x, MyPlot* plot, MyPlotItem* parent = nullptr);
	virtual ~MyPlotMarker() {
		qDebug() << "elo";
	}
	void move(const double distanceX, const double distanceY) noexcept override;
	void setColor(const QColor color) noexcept override;
	QRectF boundingRect() const noexcept override {
		auto rect = QwtPlotMarker::boundingRect();
		rect.setWidth(5);
		rect.setTop(-1000000);
		rect.setLeft(rect.left() - 2.5);
		rect.setRight(rect.right() - 2.5);
		rect.setBottom(1000000);
		return rect;
	}
};