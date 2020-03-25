#include "MyPlot.h"

QList<qreal> MyPlot::labels() const {
	QList<qreal> majorTicks;
	for (int i = 0; i <= positioner_.size(); ++i)
		majorTicks.push_back(i);
	return majorTicks;
}

MyScaleDraw * MyPlot::yAxisScaleDraw() noexcept {
	return dynamic_cast<MyScaleDraw*>(axisScaleDraw(QwtPlot::yLeft));
}

const MyScaleDraw * MyPlot::yAxisScaleDraw() const noexcept {
	return dynamic_cast<const MyScaleDraw*>(axisScaleDraw(QwtPlot::yLeft));
}

MyPlot::MyPlot(QWidget * parent) : QwtPlot(parent) {
	setFrameStyle(QFrame::NoFrame);
	setAxisScaleDraw(QwtPlot::yLeft, new MyScaleDraw(this));
	setAxisScale(QwtPlot::yLeft, 0, 1, 1);
	setCanvasBackground(QColor(Qt::black));

	(void) new CanvasManipulator(this);
	(void) new ContextMenuController(this);

	// panning with the wheel mouse button pressed
	auto panner = new MyPlotPanner(canvas());
	panner->setMouseButton(Qt::MidButton);
	connect(this, &MyPlot::wheelSignal, panner, &MyPlotPanner::wheelEvent);

	// zoom in/out with the wheel + key modifier
	auto magnifierX = new MyPlotMagnifier(canvas());
	magnifierX->setMouseButton(Qt::NoButton);
	magnifierX->setWheelModifiers(Qt::KeyboardModifier::ShiftModifier);
	magnifierX->setAxisEnabled(QwtPlot::xBottom, true);
	magnifierX->setAxisEnabled(QwtPlot::yLeft, false);

	auto magnifierY = new MyPlotMagnifier(canvas());
	magnifierY->setMouseButton(Qt::NoButton);
	magnifierY->setWheelModifiers(Qt::KeyboardModifier::AltModifier);
	magnifierY->setAxisEnabled(QwtPlot::xBottom, false);
	magnifierY->setAxisEnabled(QwtPlot::yLeft, true);

	auto magnifierBoth = new MyPlotMagnifier(canvas());
	magnifierBoth->setMouseButton(Qt::NoButton);
	magnifierBoth->setWheelModifiers(Qt::KeyboardModifier::ControlModifier);
	magnifierBoth->setAxisEnabled(QwtPlot::xBottom, true);
	magnifierBoth->setAxisEnabled(QwtPlot::yLeft, true);

	auto grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->setMajorPen(QPen(Qt::darkGray, 0, Qt::DotLine));
	grid->setMinorPen(QPen(QColor(Qt::darkGray).dark(), 0, Qt::DotLine));
	grid->attach(this);
	setAutoReplot(true);
	updatePlot();

	auto timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [this]() {forEachCurve([](MyPlotIntervalCurve* curve) {if (curve->isVisibleOnScreen())curve->refresh(); }); });
	timer->start(10);
}

std::pair<int, int> MyPlot::visibleYRange() const noexcept {
	const QRectF tr = QwtScaleMap::invTransform(xMap(), yMap(), contentsRect());
	const double yMin = tr.top();
	const double yMax = tr.bottom();
	return { yMin , yMax };
}

MyPlot::~MyPlot() {
	detachItems(0, false);
}

void MyPlot::wheelEvent(QWheelEvent * event) {
	emit wheelSignal(event);
}

void MyPlot::forEachCurve(const std::function<void(MyPlotIntervalCurve*)>& function, const std::function<bool()>& continueCondition) const noexcept {
	const QwtPlotItemList& itmList = itemList();
	for (QwtPlotItemIterator it = itmList.begin(); (it != itmList.end()) && continueCondition(); ++it) {
		if ((*it)->rtti() == QwtPlotItem::Rtti_PlotIntervalCurve)
			function(dynamic_cast<MyPlotIntervalCurve*>(*it));
	}
}

void MyPlot::forEachMyPlotItem(const std::function<void(MyPlotItem*)>& function, const std::function<bool()>& continueCondition) const noexcept {
	const QwtPlotItemList& itmList = itemList();
	for (QwtPlotItemIterator it = itmList.begin(); (it != itmList.end()) && continueCondition(); ++it) {
		if (auto myPlotItem = dynamic_cast<MyPlotItem*>(*it); myPlotItem)
			function(myPlotItem);
	}
}

MyPlotIntervalCurve * MyPlot::findCurveIf(const std::function<bool(MyPlotIntervalCurve*)> predicate) const noexcept {
	MyPlotIntervalCurve* curve = nullptr;
	forEachCurve([&curve, predicate](auto ptr) {if (predicate(ptr)) curve = ptr; }, [&curve]() { return curve == nullptr; });
	return curve;
}

std::vector<MyPlotIntervalCurve*> MyPlot::findCurvesIf(const std::function<bool(MyPlotIntervalCurve*)> predicate) const noexcept {
	std::vector<MyPlotIntervalCurve*> curves;
	curves.reserve(itemList().size());
	forEachCurve([&curves, predicate](auto ptr) {if (predicate(ptr)) curves.push_back(ptr); });
	return curves;
}

MyPlotIntervalCurve * MyPlot::findCurveByPosition(const int position) const noexcept {
	return dynamic_cast<MyPlotIntervalCurve*>(positioner_.curve(position));
}

void MyPlot::removeItems() {
	for (auto item : selection()) {
		positioner_.remove(item);
		itemsContainer_.remove(item);
	}
	updatePlot();
}

void MyPlot::updatePlot() noexcept {
	yAxisScaleDraw()->refresh();
	update();
	replot();
}

bool MyPlot::isSomethingSelected() const noexcept {
	return !selection().empty();
}

bool MyPlot::isMultipleObjectsSelected() const noexcept {
	return selection().size() > 1;
}

std::vector<MyPlotItem*> MyPlot::selection() const noexcept {
	std::vector<MyPlotItem*> selected;
	selected.reserve(itemList().size());
	forEachMyPlotItem([&selected](auto ptr) {
		if (ptr->isSelected())
			selected.push_back(ptr);
		});
	return selected;
}

void MyPlot::deselect() noexcept {
	positioner_.deselect();
	for (auto item : selection())
		item->deselect();
	updatePlot();
}

void MyPlot::setSelectedCurves(const QRect & slection) noexcept {
	auto first = valueYToPosition(slection.bottomLeft().y());
	auto last = valueYToPosition(slection.topRight().y());
	for (int i = first; i <= last; ++i) {
		auto curve = positioner_.curve(i);
		if (curve)
			curve->select();
	}
	updatePlot();
}

Positioner<MyPlotItem>& MyPlot::positioner() noexcept {
	return positioner_;
}

double MyPlot::plotXToValueX(const double x) const noexcept {
	return xMap().invTransform(x);
}

double MyPlot::plotYToValueY(const double y) const noexcept {
	return yMap().invTransform(y);
}

int MyPlot::valueYToPosition(const double y) {
	return floor(plotYToValueY(y));
}

QwtScaleMap MyPlot::xMap() const noexcept {
	return canvasMap(QwtPlot::xBottom);
}

QwtScaleMap MyPlot::yMap() const noexcept {
	return canvasMap(QwtPlot::yLeft);
}

std::vector<MyPlotIntervalCurve*> MyPlot::selectedCurves() const noexcept {
	return findCurvesIf([](MyPlotIntervalCurve* curve) { return curve->isSelected(); });
}

QPointF MyPlot::plotPointToValuePoint(const QPointF & point) const noexcept {
	return { plotXToValueX(point.x()), plotYToValueY(point.y()) };
}

QPoint MyPlot::cursorPosition() const noexcept {
	return canvas()->mapFromGlobal(QCursor::pos());
}

void MyPlot::addCurve(const QString& nameId, const DataGetterFunction& dataGetter, int type) {
	auto newCurve = std::make_unique<MyPlotIntervalCurve>(nameId, dataGetter, type == 0 ? dynamic_cast<QwtIntervalSymbol*>(new MyIntervalSymbol()) : dynamic_cast<QwtIntervalSymbol*>(new MyIntervalSymbol2()), this);
	auto position = positioner_.addExclusive(newCurve.get());
	itemsContainer_.add(std::move(newCurve));
	updatePlot();
}

void MyPlot::addMarkerAction() {
	auto marker = std::make_unique<MyPlotMarker>(100, this);
	itemsContainer_.add(std::move(marker));
}

void MyPlot::addRangeMarkersAction() {
	auto range = std::make_unique<PlotMarkerPair>(20, 300, this);
	itemsContainer_.add(std::move(range));
}
