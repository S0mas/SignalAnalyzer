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

enum class SignalCurveType {
	SingleBitSignal = 0,
	ComplexSignal_Block,
	ComplexSignal_Wave
};



class MyPlot : public QwtPlot {
	Q_OBJECT
	QList<qreal> labels() const;
	MyScaleDraw* yAxisScaleDraw() noexcept;
	const MyScaleDraw* yAxisScaleDraw() const noexcept;

	Positioner<MyPlotItem> positioner_;
	PlotItemsContainer itemsContainer_;
	QTimer* refreshTimer_ = new QTimer(this);
public:
	MyPlot(QWidget* parent = nullptr);
	virtual ~MyPlot();
	std::pair<int, int> visibleYRange() const noexcept;
	void wheelEvent(QWheelEvent *event) override;
	void forEachCurve(const std::function<void(MyPlotAbstractCurve*)>& function, const std::function<bool()>& continueCondition = []() { return true; }) const noexcept;
	void forEachMyPlotItem(const std::function<void(MyPlotItem*)>& function, const std::function<bool()>& continueCondition = []() { return true; }) const noexcept;
	MyPlotAbstractCurve* findCurveIf(const std::function<bool(MyPlotAbstractCurve*)> predicate) const noexcept;
	std::vector<MyPlotAbstractCurve*> findCurvesIf(const std::function<bool(MyPlotAbstractCurve*)> predicate) const noexcept;
	MyPlotAbstractCurve* findCurveByPosition(const int position) const noexcept;
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
	int plotYToPosition(const double y);
	QwtScaleMap xMap() const noexcept;
	QwtScaleMap yMap() const noexcept;
	std::vector<MyPlotAbstractCurve*> selectedCurves() const noexcept;
	QPointF plotPointToValuePoint(const QPointF& point) const noexcept;
	QPoint cursorPosition() const noexcept;
	MyPlotAbstractCurve* addCurve(const QString& nameId, SignalCurveType const type, bool const isRealTimeDataSource = true, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& initialData = {});
	void addCurve(std::unique_ptr<MyPlotAbstractCurve>&& curve);
	void setRefreshTimeInterval(uint32_t const ms) noexcept;
	uint32_t refreshTimeInterval() const noexcept;
private slots:
	void refreshVisibleCurves() const;
public slots:
	void addMarkerAction();
	void addRangeMarkersAction();
signals:
	void addCurveActionStarted() const;
	void wheelSignal(QWheelEvent*) const;
	void addedItem(QStandardItem*) const;
};
