#pragma once
#include "MyPlotIntervalCurve.h"
#include "MyPlot.h"

static inline QVector<QwtIntervalSample> convertToQwtIntervalImpl(const std::vector<double>& signalsData) noexcept {
	QVector<QwtIntervalSample> vec;
	vec.reserve(100);
	double samplesNo = signalsData.size();
	for (double sampleIndex = 0; sampleIndex < samplesNo;) {
		double tempVal = signalsData[sampleIndex];
		double length = 1;
		while (++sampleIndex < samplesNo && signalsData[sampleIndex] == tempVal)
			++length;
		vec.push_back({ tempVal, sampleIndex - length, sampleIndex });
	}
	return vec;
}

MyPlotIntervalCurve::MyPlotIntervalCurve(const QString& nameId, const DataGetterFunction& dataGetter, QwtIntervalSymbol* symbol, MyPlot* plot) : QwtPlotIntervalCurve(nameId), MyPlotItem(plot) {
	nameId_ = nameId;
	isPositionedExclusive_ = true;
	setStyle(CurveStyle::NoCurve);
	dataGetter_ = dataGetter;
	setSamples(convertToQwtIntervalImpl(dataGetter()));
	setOrientation(Qt::Orientation::Horizontal);

	QBrush brush(Qt::BrushStyle::SolidPattern);
	brush.setColor(Qt::black);
	symbol->setBrush(brush);
	setSymbol(symbol);

	mainColor_ = QColor(Qt::green);
	setColor(mainColor_);
	QwtPlotIntervalCurve::attach(plot);
}

MyPlotIntervalCurve::~MyPlotIntervalCurve() {}

void MyPlotIntervalCurve::drawSymbols(QPainter* painter, const QwtIntervalSymbol& symbol, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
	const QRectF& canvasRect, int from, int to) const {
	if (isVisibleOnScreen()) {
		painter->save();
		QPen pen = symbol.pen();
		pen.setCapStyle(Qt::FlatCap);
		painter->setPen(pen);
		painter->setBrush(symbol.brush());
		auto mySymbol1 = dynamic_cast<const MyIntervalSymbol*>(&symbol);
		auto mySymbol2 = dynamic_cast<const MyIntervalSymbol2*>(&symbol);

		bool lastValue = false;
		for (int i = from; i <= to; i++) {
			auto const s = sample(i);
			const double x1 = xMap.transform(s.interval.minValue());
			const double x2 = xMap.transform(s.interval.maxValue());
			const double y1 = yMap.transform(position());
			auto size = abs(plot_->yMap().transform(1) - plot_->yMap().transform(0)) * 0.9;
			if (mySymbol1)
				mySymbol1->draw(painter, orientation(), QPointF(x1, y1), QPointF(x2, y1), s.value, size);
			else if (mySymbol2) {
				mySymbol2->draw(painter, orientation(), QPointF(x1, y1), QPointF(x2, y1), static_cast<bool>(s.value), size, lastValue != static_cast<bool>(s.value));
				lastValue = static_cast<bool>(s.value);
			}
		}

		painter->restore();
	}
}

double MyPlotIntervalCurve::position() const noexcept {
	return plot_->positioner().position(this);
}

void MyPlotIntervalCurve::move(const double distanceX, const double distanceY) noexcept {}

void MyPlotIntervalCurve::setColor(const QColor color) noexcept {
	auto p = pen();
	p.setColor(color);
	const_cast<QwtIntervalSymbol*>(symbol())->setPen(p); // it should not cause undefined behavior because symbol object is not really const
}

void MyPlotIntervalCurve::refresh() noexcept {
	setSamples(convertToQwtIntervalImpl(dataGetter_()));
}

QString MyPlotIntervalCurve::nameId() const noexcept {
	return nameId_;
}

static inline bool qwtIsHSampleInside(const QwtIntervalSample &sample,
	double xMin, double xMax, double yMin, double yMax) {
	const double y = sample.value;
	const double x1 = sample.interval.minValue();
	const double x2 = sample.interval.maxValue();

	const bool isOffScreen = (y < yMin) || (y > yMax)
		|| (x1 < xMin && x2 < xMin) || (x1 > xMax && x2 > xMax);

	return !isOffScreen;
}

bool MyPlotIntervalCurve::isVisibleOnScreen() const noexcept {
	auto const range = plot_->visibleYRange();
	auto const pos = position();
	return pos >= range.first && pos <= range.second;
}