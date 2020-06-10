#pragma once
#include "MyPlotCurve.h"
#include "MyPlot.h"
#include "qwt_painter.h"
#include "qwt_point_mapper.h"
#include "qwt_clipper.h"
#include "qwt_curve_fitter.h"

static QVector<QPointF> convertToQwtIntervalImpl(double const position, const std::vector<double>& signalsData) noexcept {
	double max = std::numeric_limits<double>::min();
	double min = std::numeric_limits<double>::max();
	for (auto const& sample : signalsData) {
		if (sample > max)
			max = sample;
		if (sample < min)
			min = sample;
	}
	auto scaleFunc = [min, max](double const sample) { return ((sample - min) / (max - min)); };
	QVector<QPointF> data;
	data.reserve(signalsData.size());
	int x = 0; //TODO use Timestamp data
	for (auto const& sample : signalsData)
		data.push_back(QPointF(x++, position + scaleFunc(sample)));
	return data;
}

MyPlotCurve::MyPlotCurve(const QString& nameId, MyPlot* plot, bool const isRealTimeCurve) : QwtPlotCurve(nameId), MyPlotAbstractCurve(plot, isRealTimeCurve) {
	nameId_ = nameId;
	isPositionedExclusive_ = true;
	setStyle(CurveStyle::Lines);
	setOrientation(Qt::Orientation::Horizontal);

	QBrush brush(Qt::BrushStyle::SolidPattern);
	brush.setColor(Qt::black);
	setBrush(brush);
	static int color = 5;
	mainColor_ = QColor(static_cast<Qt::GlobalColor>(color++));

	setColor(mainColor_);
	QwtPlotCurve::attach(plot);
}

void MyPlotCurve::drawLines(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect, int from, int to) const {
	if (from > to)
		return;

	const bool doAlign = QwtPainter::roundingAlignment(painter);
	const bool doFit = (testCurveAttribute(Fitted) && curveFitter());
	const bool doFill = (brush().style() != Qt::NoBrush)
		&& (brush().color().alpha() > 0);

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
	if (doFill) {
		if (painter->pen().style() != Qt::NoPen) {
			// here we are wasting memory for the filled copy,
			// do polygon clipping twice etc .. TODO
			QPolygonF filled = polyline;
			fillCurve(painter, xMap, yMap, canvasRect, filled);
			filled.clear();
			if (testPaintAttribute(ClipPolygons))
				polyline = QwtClipper::clipPolygonF(clipRect, polyline, false);
			QwtPainter::drawPolyline(painter, polyline);
		}
		else
			fillCurve(painter, xMap, yMap, canvasRect, polyline);
	}
	else {
		if (testPaintAttribute(ClipPolygons))
			polyline = QwtClipper::clipPolygonF(clipRect, polyline, false);
		QwtPainter::drawPolyline(painter, polyline);
	}
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

std::optional<double> MyPlotCurve::value(int32_t const x) const noexcept {
	if(realData_.size() > x)
		return realData_[x];
	return std::nullopt;
}

void MyPlotCurve::handleData(std::vector<double> const& data) {
	realData_ = data;
	setSamples(convertToQwtIntervalImpl(position(), data));
}