#pragma once

#include <QPainter>
#include <QPoint>

#include "qwt_plot_curve.h"
#include "qwt_scale_map.h"
#include "MyPlotItem.h"

class MyPlotCurve : public MyPlotAbstractCurve, public QwtPlotCurve {
	Q_OBJECT
	uint32_t xResolution_ = 1;//if timestamps are enabled then it depends on the scan rate of the acq
	double min_ = std::numeric_limits<double>::max();
	double max_ = std::numeric_limits<double>::min();
	double extrapolate(QPointF const& p1, QPointF const& p2, double const toFind) const noexcept;
	double scale(double const value) const noexcept;
	double unscale(double const value) const noexcept;
	void calculateScale(const std::vector<double>& signalsData) noexcept;
	QVector<QPointF> convertToSamples(double const position, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) noexcept;
public:
	MyPlotCurve(const QString& nameId, MyPlot* plot, bool const isRealTimeCurve = true);
	~MyPlotCurve() override = default;
	void drawLines(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const override;
	double position() const noexcept;
	void move(const double distanceX, const double distanceY) noexcept;
	void setColor(const QColor color) noexcept;
	QRectF boundingRect() const noexcept override;
	bool isVisibleOnScreen() const noexcept override;
	std::optional<double> value(double const x) const noexcept override;
public slots:
	void handleData(std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) override;
};