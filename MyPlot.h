#pragma once
#include "MyScaleDraw.h"
#include "CanvasMover.h"
#include "CanvasSelector.h"
#include "ContextMenuController.h"
#include "MyPlotIntervalCurve.h"
#include "MyPlotItem.h"
#include "MyPlotMagnifier.h"
#include "MyPlotPanner.h"
#include "MyPickerDragRectMachine.h"
#include "MyPlotMarker.h"
#include "PlotRangeMarkers.h"
#include "CurvesPositioner.h"
#include "CanvasManipulator.h"
#include "PlotItemsContainer.h"

#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QObject>

#include <qwt_event_pattern.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>

class MyPlot : public QwtPlot {
	Q_OBJECT

	QList<qreal> labels() const {
		QList<qreal> majorTicks;
		for (int i = 0; i <= positioner_.size(); ++i)
			majorTicks.push_back(i);
		return majorTicks;
	}

	MyScaleDraw* yAxisScaleDraw() noexcept {
		return dynamic_cast<MyScaleDraw*>(axisScaleDraw(QwtPlot::yLeft));
	}

	const MyScaleDraw* yAxisScaleDraw() const noexcept {
		return dynamic_cast<const MyScaleDraw*>(axisScaleDraw(QwtPlot::yLeft));
	}

	Positioner<MyPlotItem> positioner_;
	PlotItemsContainer itemsContainer_;
	std::set<Position> selectedPositions_;
public:
	MyPlot(QWidget* parent = nullptr) : QwtPlot(parent) {
		setFrameStyle(QFrame::NoFrame);
		setAxisScaleDraw(QwtPlot::yLeft, new MyScaleDraw(this));
		setAxisScale(QwtPlot::yLeft, 0, 1, 1);
		setAxisFont(QwtPlot::xBottom, QFont("courier", 8));
		QwtText xText("Seconds");
		xText.setFont(QFont("arial", 10, QFont::Bold));
		setAxisTitle(QwtPlot::xBottom, xText);
		setCanvasBackground(QColor(Qt::black));

		//(void) new CanvasSelector(this);
		//(void) new CanvasMover(this);
		(void) new CanvasManipulator(this);
		(void) new ContextMenuController(this);

		// panning with the wheel mouse button pressed
		auto panner = new MyPlotPanner(canvas());
		panner->setMouseButton(Qt::MidButton);

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
	}

	virtual ~MyPlot() {
		detachItems(0, false);
	}

	void forEachCurve(const std::function<void(MyPlotIntervalCurve*)>& function, const std::function<bool()>& continueCondition = []() { return true; }) const noexcept {
		const QwtPlotItemList& itmList = itemList();
		for (QwtPlotItemIterator it = itmList.begin(); (it != itmList.end()) && continueCondition(); ++it) {
			if ((*it)->rtti() == QwtPlotItem::Rtti_PlotIntervalCurve)
				function(dynamic_cast<MyPlotIntervalCurve*>(*it));
		}
	}

	void forEachMyPlotItem(const std::function<void(MyPlotItem*)>& function, const std::function<bool()>& continueCondition = []() { return true; }) const noexcept {
		const QwtPlotItemList& itmList = itemList();
		for (QwtPlotItemIterator it = itmList.begin(); (it != itmList.end()) && continueCondition(); ++it) {		
			if(auto myPlotItem = dynamic_cast<MyPlotItem*>(*it); myPlotItem)
				function(myPlotItem);
		}
	}

	MyPlotIntervalCurve* findCurveIf(const std::function<bool(MyPlotIntervalCurve*)> predicate) const noexcept {
		MyPlotIntervalCurve* curve = nullptr;
		forEachCurve([&curve, predicate](auto ptr) {if (predicate(ptr)) curve = ptr; }, [&curve]() { return curve == nullptr; });
		return curve;
	}

	std::vector<MyPlotIntervalCurve*> findCurvesIf(const std::function<bool(MyPlotIntervalCurve*)> predicate) const noexcept {
		std::vector<MyPlotIntervalCurve*> curves;
		curves.reserve(itemList().size());
		forEachCurve([&curves, predicate](auto ptr) {if (predicate(ptr)) curves.push_back(ptr); });
		return curves;
	}

	MyPlotIntervalCurve* findCurveByPosition(const int position) const noexcept {
		return dynamic_cast<MyPlotIntervalCurve*>(positioner_.curve(position));
	}

	void removeItems() {
		for (auto item : selection()) {
			positioner_.remove(item);
			itemsContainer_.remove(item);
		}	
		updatePlot();
	}

	void updatePlot() noexcept {
		yAxisScaleDraw()->refresh();
		update();
		replot();
	}

	bool isSomethingSelected() const noexcept {
		return !selection().empty();
	}

	bool isMultipleObjectsSelected() const noexcept {
		return selection().size() > 1;
	}

	std::vector<MyPlotItem*> selection() const noexcept {
		std::vector<MyPlotItem*> selected;
		selected.reserve(itemList().size());
		forEachMyPlotItem([&selected](auto ptr) {
			if (ptr->isSelected())
				selected.push_back(ptr);
		});
		return selected;
	}

	void deselect() noexcept {
		positioner_.deselect();
		selectedPositions_.clear();
		for (auto item : selection())
			item->deselect();
		updatePlot();
	}

	void setSelectedCurves(const QRect& slection) noexcept {
		auto first = valueYToPosition(slection.bottomLeft().y());
		auto last = valueYToPosition(slection.topRight().y());
		for (int i = first; i <= last; ++i) {
			auto curve = positioner_.curve(i);
			if(curve)
				curve->select();
		}
		updatePlot();
	}

	Positioner<MyPlotItem>& positioner() noexcept {
		return positioner_;
	}

	double plotXToValueX(const double x) const noexcept {
		return xMap().invTransform(x);
	}

	double plotYToValueY(const double y) const noexcept {
		return yMap().invTransform(y);
	}

	int valueYToPosition(const double y) {
		return floor(plotYToValueY(y));
	}

	QwtScaleMap xMap() const noexcept {
		return canvasMap(QwtPlot::xBottom);
	}

	QwtScaleMap yMap() const noexcept {
		return canvasMap(QwtPlot::yLeft);
	}

	std::vector<MyPlotIntervalCurve*> selectedCurves() const noexcept {
		return findCurvesIf([](MyPlotIntervalCurve* curve) { return curve->isSelected(); });
	}

	QPointF plotPointToValuePoint(const QPointF& point) const noexcept {
		return { plotXToValueX(point.x()), plotYToValueY(point.y()) };
	}

	QPoint cursorPosition() const noexcept {
		return canvas()->mapFromGlobal(QCursor::pos());
	}
public slots:
	void addCurve(const QString& nameId, const DataGetterFunction& dataGetter, int type) {
		auto newCurve = std::make_unique<MyPlotIntervalCurve>(nameId, dataGetter, type == 0 ? dynamic_cast<QwtIntervalSymbol*>(new MyIntervalSymbol()) : dynamic_cast<QwtIntervalSymbol*>(new MyIntervalSymbol2()), this);
		auto position = positioner_.addExclusive(newCurve.get());
		itemsContainer_.add(std::move(newCurve));
		updatePlot();
	}

	void addMarkerAction() {
		auto marker = std::make_unique<MyPlotMarker>(100, this);
		itemsContainer_.add(std::move(marker));
	}

	void addRangeMarkersAction() {
		auto range = std::make_unique<PlotMarkerPair>(20, 300, this);
		itemsContainer_.add(std::move(range));
	}
signals:
	void addCurveActionStarted() const;
};
