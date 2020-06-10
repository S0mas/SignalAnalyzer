#pragma once

#include <QPainter>
#include <QPoint>

#include "qwt_plot_intervalcurve.h"
#include "qwt_scale_map.h"

#include "MyIntervalSymbol.h"
#include "MyPlotItem.h"

class MyPlotIntervalCurve : public MyPlotAbstractCurve, public QwtPlotIntervalCurve {
	Q_OBJECT
public:
	MyPlotIntervalCurve(const QString& nameId, QwtIntervalSymbol* symbol, MyPlot* plot, bool const isRealTimeCurve = true);
	~MyPlotIntervalCurve() override = default;
	void drawSymbols(QPainter*, const QwtIntervalSymbol&,const QwtScaleMap&, const QwtScaleMap&, const QRectF&, int, int) const override;
	void drawTube(QPainter*, const QwtScaleMap&, const QwtScaleMap&, const QRectF&, int, int) const override {}

	void move(const double distanceX, const double distanceY) noexcept override;
	void setColor(const QColor color) noexcept override;
	QRectF boundingRect() const noexcept override;
	bool isVisibleOnScreen() const noexcept override;
	double position() const noexcept;
	std::optional<double> value(int32_t const x) const noexcept override;
public slots:
	void handleData(std::vector<double> const& data) override;
};