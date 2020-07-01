#include "MyPlot.h"
#include "MyPlotCurve.h"

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
	magnifierX->setAxisEnabled(QwtPlot::xBottom, false);
	magnifierX->setAxisEnabled(QwtPlot::yLeft, true);

	auto magnifierY = new MyPlotMagnifier(canvas());
	magnifierY->setMouseButton(Qt::NoButton);
	magnifierY->setWheelModifiers(Qt::KeyboardModifier::AltModifier);
	magnifierY->setAxisEnabled(QwtPlot::xBottom, true);
	magnifierY->setAxisEnabled(QwtPlot::yLeft, false);

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

	connect(refreshTimer_, &QTimer::timeout, this, &MyPlot::refreshVisibleCurves);
	refreshTimer_->start(100);
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

void MyPlot::forEachCurve(const std::function<void(MyPlotAbstractCurve*)>& function, const std::function<bool()>& continueCondition) const noexcept {
	const QwtPlotItemList& itmList = itemList();
	for (QwtPlotItemIterator it = itmList.begin(); (it != itmList.end()) && continueCondition(); ++it) {
		if ((*it)->rtti() == QwtPlotItem::Rtti_PlotIntervalCurve || (*it)->rtti() == QwtPlotItem::Rtti_PlotCurve)
			function(dynamic_cast<MyPlotAbstractCurve*>(*it));
	}
}

void MyPlot::forEachMyPlotItem(const std::function<void(MyPlotItem*)>& function, const std::function<bool()>& continueCondition) const noexcept {
	const QwtPlotItemList& itmList = itemList();
	for (QwtPlotItemIterator it = itmList.begin(); (it != itmList.end()) && continueCondition(); ++it) {
		if (auto myPlotItem = dynamic_cast<MyPlotItem*>(*it); myPlotItem)
			function(myPlotItem);
	}
}

MyPlotAbstractCurve* MyPlot::findCurveIf(const std::function<bool(MyPlotAbstractCurve*)> predicate) const noexcept {
	MyPlotAbstractCurve* curve = nullptr;
	forEachCurve([&curve, predicate](auto ptr) {if (predicate(ptr)) curve = ptr; }, [&curve]() { return curve == nullptr; });
	return curve;
}

std::vector<MyPlotAbstractCurve*> MyPlot::findCurvesIf(const std::function<bool(MyPlotAbstractCurve*)> predicate) const noexcept {
	std::vector<MyPlotAbstractCurve*> curves;
	curves.reserve(itemList().size());
	forEachCurve([&curves, predicate](auto ptr) {if (predicate(ptr)) curves.push_back(ptr); });
	return curves;
}

MyPlotAbstractCurve* MyPlot::findCurveByPosition(const int position) const noexcept {
	return dynamic_cast<MyPlotAbstractCurve*>(positioner_.curve(position));
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
	auto first = plotYToPosition(slection.bottomLeft().y());
	auto last = plotYToPosition(slection.topRight().y());
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

int MyPlot::plotYToPosition(const double y) {
	return floor(plotYToValueY(y));
}

QwtScaleMap MyPlot::xMap() const noexcept {
	return canvasMap(QwtPlot::xBottom);
}

QwtScaleMap MyPlot::yMap() const noexcept {
	return canvasMap(QwtPlot::yLeft);
}

std::vector<MyPlotAbstractCurve*> MyPlot::selectedCurves() const noexcept {
	return findCurvesIf([](MyPlotAbstractCurve* curve) { return curve->isSelected(); });
}

QPointF MyPlot::plotPointToValuePoint(const QPointF & point) const noexcept {
	return { plotXToValueX(point.x()), plotYToValueY(point.y()) };
}

QPoint MyPlot::cursorPosition() const noexcept {
	return canvas()->mapFromGlobal(QCursor::pos());
}

MyPlotAbstractCurve* MyPlot::addCurve(const QString& nameId, SignalCurveType const type, bool const isRealTimeDataSource, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& initialData) {
	std::unique_ptr<MyPlotAbstractCurve> item;
	if(type == SignalCurveType::SingleBitSignal)
		item = std::make_unique<MyPlotIntervalCurve>(nameId, dynamic_cast<QwtIntervalSymbol*>(new MyIntervalSymbol2()), this, isRealTimeDataSource);
	if(type == SignalCurveType::ComplexSignal_Block)
		item = std::make_unique<MyPlotIntervalCurve>(nameId, dynamic_cast<QwtIntervalSymbol*>(new MyIntervalSymbol()), this, isRealTimeDataSource);
	if (type == SignalCurveType::ComplexSignal_Wave)
		item = std::make_unique<MyPlotCurve>(nameId, this, isRealTimeDataSource);
	auto position = positioner_.addExclusive(item.get());
	auto curve = item.get();
	if(!initialData.first.empty())
		curve->handleData(initialData);
	itemsContainer_.add(std::move(item));
	updatePlot();
	return curve;
}

void MyPlot::addCurve(std::unique_ptr<MyPlotAbstractCurve>&& curve) {
	auto position = positioner_.addExclusive(curve.get());
	itemsContainer_.add(std::move(curve));
	updatePlot();
}

void MyPlot::refreshVisibleCurves() const {
	forEachCurve([](MyPlotAbstractCurve* curve) {if (curve->isRealTimeCurve() && curve->isVisibleOnScreen()) emit curve->dataRequest(); });
}

void MyPlot::addMarkerAction() {
	auto marker = std::make_unique<MyPlotMarker>(100, this);
	itemsContainer_.add(std::move(marker));
}

void MyPlot::addRangeMarkersAction() {
	auto range = std::make_unique<PlotMarkerPair>(20, 300, this);
	itemsContainer_.add(std::move(range));
}

void MyPlot::setRefreshTimeInterval(uint32_t ms) noexcept {
	refreshTimer_->setInterval(ms);
}

uint32_t MyPlot::refreshTimeInterval() const noexcept {
	return refreshTimer_->interval();
}
