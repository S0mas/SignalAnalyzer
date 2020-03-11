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
	MyPlotIntervalCurve(const QString& nameId, const DataGetterFunction& dataGetter, QwtIntervalSymbol* symbol, MyPlot* plot);
	virtual ~MyPlotIntervalCurve();
	void drawSymbols(QPainter* painter, const QwtIntervalSymbol& symbol,const QwtScaleMap& xMap, const QwtScaleMap& yMap,
		const QRectF& canvasRect, int from, int to) const override;
	void move(const QPointF& destination) noexcept override;
	void setColor(const QColor color) noexcept override;
	QString nameId() const noexcept;
	QRectF boundingRect() const noexcept override {
		auto rect = QwtPlotIntervalCurve::boundingRect();
		rect.setBottom(position());
		rect.setTop(position()+1);
		return rect;
	}

	void remove() noexcept override {
		detach();
	}

	double MyPlotIntervalCurve::position() const noexcept;

	void refresh() noexcept;
};