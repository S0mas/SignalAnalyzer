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
#include <QStandardItem>

#include <qwt_event_pattern.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_widget.h>
#include <qwt_wheel.h>

#include <QTimer>

class MyPlot : public QwtPlot {
	Q_OBJECT
	QList<qreal> labels() const;
	MyScaleDraw* yAxisScaleDraw() noexcept;
	const MyScaleDraw* yAxisScaleDraw() const noexcept;

	Positioner<MyPlotItem> positioner_;
	PlotItemsContainer itemsContainer_;
public:
	MyPlot(QWidget* parent = nullptr);
	virtual ~MyPlot();
	std::pair<int, int> visibleYRange() const noexcept;
	void wheelEvent(QWheelEvent *event) override;
	void forEachCurve(const std::function<void(MyPlotIntervalCurve*)>& function, const std::function<bool()>& continueCondition = []() { return true; }) const noexcept;
	void forEachMyPlotItem(const std::function<void(MyPlotItem*)>& function, const std::function<bool()>& continueCondition = []() { return true; }) const noexcept;
	MyPlotIntervalCurve* findCurveIf(const std::function<bool(MyPlotIntervalCurve*)> predicate) const noexcept;
	std::vector<MyPlotIntervalCurve*> findCurvesIf(const std::function<bool(MyPlotIntervalCurve*)> predicate) const noexcept;
	MyPlotIntervalCurve* findCurveByPosition(const int position) const noexcept;
	void removeItems();
	void updatePlot() noexcept;
	bool isSomethingSelected() const noexcept;
	bool isMultipleObjectsSelected() const noexcept;
	std::vector<MyPlotItem*> selection() const noexcept;
	void deselect() noexcept;
	void setSelectedCurves(const QRect& slection) noexcept;
	Positioner<MyPlotItem>& positioner() noexcept;
	double plotXToValueX(const double x) const noexcept;
	double plotYToValueY(const double y) const noexcept;
	int valueYToPosition(const double y);
	QwtScaleMap xMap() const noexcept;
	QwtScaleMap yMap() const noexcept;
	std::vector<MyPlotIntervalCurve*> selectedCurves() const noexcept;
	QPointF plotPointToValuePoint(const QPointF& point) const noexcept;
	QPoint cursorPosition() const noexcept;
public slots:
	void addCurve(const QString& nameId, const DataGetterFunction& dataGetter, int type);
	void addMarkerAction();
	void addRangeMarkersAction();
signals:
	void addCurveActionStarted() const;
	void wheelSignal(QWheelEvent*) const;
	void addedItem(QStandardItem*) const;
};
