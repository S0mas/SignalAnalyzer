#pragma once
#include "MyPlotCurve.h"
#include "MyPlot.h"
#include "qwt_painter.h"
#include "qwt_point_mapper.h"
#include "qwt_clipper.h"
#include "qwt_curve_fitter.h"

static inline QString getColor(int const i) noexcept {
	static std::array<QString, 32> colors = { "#0000ff", "#8a2be2", "#a52a2a", "#deb887", "#5f9ea0", "#7fff00", "#d2691e", "#6495ed",
											 "#dc143c", "#a9a9a9", "#006400", "#8b008b", "#8b0000", "#8fbc8f", "#ff1493", "#1e90ff",
											 "#ffd700", "#adff2f", "#cd5c5c", "#20b2aa", "#ffb6c1", "#66cdaa", "#00fa9a", "#ffe4b5",
											 "#9acd32", "#008080", "#a0522d", "#fa8072", "#b0e0e6", "#ffffff", "#2e8b57", "#bc8f8f" };
	return colors[i % 32];
}

void MyPlotCurve::calculateScale(const std::vector<double>& signalsData) noexcept {
	double max = std::numeric_limits<double>::min();
	double min = std::numeric_limits<double>::max();
	for (auto const& sample : signalsData) {
		if (sample > max)
			max = sample;
		if (sample < min)
			min = sample;
	}
	//only update scale if min/max values are not in the apresent range or are ~30% hihger/lower.
	if (max > max_ || ((max_ > 0) && (max < 0.7 * max_)) || ((max_ <= 0) && (max < 1.3 * max_)))
		max_ = max;
	if (min < min_ || ((min_ > 0) && (min > 1.3 * min_)) || ((min_ <= 0) && (min > 0.7 * min_)))
		min_ = min;
}

double MyPlotCurve::unscale(double const value) const noexcept {
	return (value - position()) * (max_ - min_) + min_;
}

double MyPlotCurve::scale(double const sample) const noexcept {
	return (sample - min_) / (max_ - min_) + position();
}

QVector<QPointF> MyPlotCurve::convertToSamples(double const position, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) noexcept {
	auto const& [signalData, timestamps] = data;
	calculateScale(signalData);
	QVector<QPointF> samples;
	samples.resize(data.first.size());

	//TODO remove this condition check replace with some better one
	if (timestamps[0].seconds_ == timestamps[1].seconds_ && timestamps[0].nanoseconds_ == timestamps[1].nanoseconds_)
		std::transform(signalData.begin(), signalData.end(), samples.begin(), [x = 0, this](double const sample) mutable { return QPointF(x++, scale(sample)); });
	else //TODO use also the nanoseconds
		std::transform(signalData.begin(), signalData.end(), samples.begin(), [i = 0, timestamps, this](double const sample) mutable { return QPointF(timestamps[i++].seconds_ - timestamps[0].seconds_, scale(sample)); });

	return samples;
}

MyPlotCurve::MyPlotCurve(const QString& nameId, MyPlot* plot, bool const isRealTimeCurve) : QwtPlotCurve(nameId), MyPlotAbstractCurve(plot, isRealTimeCurve) {
	nameId_ = nameId;
	isPositionedExclusive_ = true;
	setStyle(CurveStyle::Lines);
	setOrientation(Qt::Orientation::Horizontal);

	static int color = 0;
	mainColor_ = getColor(color++);

	setColor(mainColor_);
	QwtPlotCurve::attach(plot);
}

void MyPlotCurve::drawLines(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const {
	if (from > to)
		return;
	const bool doAlign = QwtPainter::roundingAlignment(painter);
	const bool doFit = (testCurveAttribute(Fitted) && curveFitter());

	QRectF clipRect;
	if (testPaintAttribute(ClipPolygons)) {
		qreal pw = qMax(qreal(1.0), painter->pen().widthF());
		clipRect = canvasRect.adjusted(-pw, -pw, pw, pw);
	}
	const bool noDuplicates = testPaintAttribute(FilterPoints);
	QwtPointMapper mapper;
	mapper.setFlag(QwtPointMapper::RoundPoints, doAlign);
	mapper.setFlag(QwtPointMapper::WeedOutPoints, noDuplicates);
	mapper.setBoundingRect(canvasRect);

	QPolygonF polyline = mapper.toPolygonF(xMap, yMap, data(), from, to);
	if (doFit)
		polyline = curveFitter()->fitCurve(polyline);
	if (testPaintAttribute(ClipPolygons))
		polyline = QwtClipper::clipPolygonF(clipRect, polyline, false);
	QwtPainter::drawPolyline(painter, polyline);
}

double MyPlotCurve::position() const noexcept {
	return plot_->positioner().position(this);
}

void MyPlotCurve::move(const double distanceX, const double distanceY) noexcept {}

void MyPlotCurve::setColor(const QColor color) noexcept {
	auto p = pen();
	p.setColor(color);
	setPen(p);
}

QRectF MyPlotCurve::boundingRect() const noexcept {
	auto rect = QwtPlotCurve::boundingRect();
	rect.setBottom(position());
	rect.setTop(position() + 1);
	return rect;
}

static inline bool qwtIsHSampleInside(const QwtIntervalSample& sample,
	double xMin, double xMax, double yMin, double yMax) {
	const double y = sample.value;
	const double x1 = sample.interval.minValue();
	const double x2 = sample.interval.maxValue();

	const bool isOffScreen = (y < yMin) || (y > yMax)
		|| (x1 < xMin && x2 < xMin) || (x1 > xMax&& x2 > xMax);

	return !isOffScreen;
}

bool MyPlotCurve::isVisibleOnScreen() const noexcept {
	auto const range = plot_->visibleYRange();
	auto const pos = position();
	return pos >= range.first && pos <= range.second;
}

double MyPlotCurve::extrapolate(QPointF const& p1, QPointF const& p2, double const toFind) const noexcept {
	return ((toFind - p1.x()) / (p2.x() - p1.x())) * (p2.y() - p1.y()) + p1.y();
}

std::optional<double> MyPlotCurve::value(double const x) const noexcept {
	auto const& d = data();
	if (d->size() > 0 && (x < d->sample(0).x() || x > d->sample(d->size() - 1).x()))
		return std::nullopt;
	QPointF closestSampleWithLessX;
	for (int i = 0; i < d->size(); ++i) {
		auto const& sample = d->sample(i);
		if (sample.x() == x)
			return unscale(sample.y()); // exact sample found
		if (sample.x() < x)
			closestSampleWithLessX = sample;
		if (sample.x() > x)
			return unscale(extrapolate(closestSampleWithLessX, sample, x));
	}
	return std::nullopt;//shouldnt ever execute
}

void MyPlotCurve::handleData(std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) {
	setSamples(convertToSamples(position(), data));
}