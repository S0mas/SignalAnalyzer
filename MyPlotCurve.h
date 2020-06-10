#pragma once

#include <QPainter>
#include <QPoint>

#include "qwt_plot_curve.h"
#include "qwt_scale_map.h"
#include "MyPlotItem.h"

class MyPlotCurve : public MyPlotAbstractCurve, public QwtPlotCurve {
	Q_OBJECT
	QString nameId_;
	std::vector<double> realData_;
public:
	MyPlotCurve(const QString& nameId, MyPlot* plot, bool const isRealTimeCurve = true);
	~MyPlotCurve() override = default;
	void drawLines(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const override;
	double position() const noexcept;
	void move(const double distanceX, const double distanceY) noexcept;
	void setColor(const QColor color) noexcept;
	QRectF boundingRect() const noexcept override;
	bool isVisibleOnScreen() const noexcept override;
	std::optional<double> value(int32_t const x) const noexcept override;
public slots:
	void handleData(std::vector<double> const& data) override;
};