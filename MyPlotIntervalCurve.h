#pragma once

#include <QPainter>
#include <QPoint>

#include "qwt_plot_intervalcurve.h"
#include "qwt_scale_map.h"

#include "MyIntervalSymbol.h"
#include "MyPlotItem.h"

using DataGetterFunction = std::function<std::vector<double>()>;

class MyPlotIntervalCurve : public QwtPlotIntervalCurve, public MyPlotItem {
	DataGetterFunction dataGetter_;
	QString nameId_;

public:
	inline static int counter = 0;
	MyPlotIntervalCurve(const QString& nameId, const DataGetterFunction& dataGetter, QwtIntervalSymbol* symbol, MyPlot* plot);
	virtual ~MyPlotIntervalCurve();
	void drawSymbols(QPainter*, const QwtIntervalSymbol&,const QwtScaleMap&, const QwtScaleMap&, const QRectF&, int, int) const override;
	void drawTube(QPainter*, const QwtScaleMap&, const QwtScaleMap&, const QRectF&, int, int) const override {}

	void move(const double distanceX, const double distanceY) noexcept override;
	void setColor(const QColor color) noexcept override;
	QString nameId() const noexcept;
	QRectF boundingRect() const noexcept override {
		auto rect = QwtPlotIntervalCurve::boundingRect();
		rect.setBottom(position());
		rect.setTop(position()+1);
		return rect;
	}

	bool isVisibleOnScreen() const noexcept;
	double position() const noexcept;

	void refresh() noexcept;
};