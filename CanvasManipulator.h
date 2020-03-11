#pragma once
#include <QObject>
#include <QPointF>

class MyPlot;
class MyPlotIntervalCurve;
class MyPlotItem;
class MyPlotPicker;
class QEvent;
class QPoint;
class QRectF;

enum class STATE {
	NO_SELECTION,
	SELECTING,
	SELECTED,
	MOVING
};

class CanvasManipulator : public QObject {
	Q_OBJECT
	MyPlotPicker* picker_;
	QPointF moveStart_;
	STATE state_;
public:
	CanvasManipulator(MyPlot* plot);
	bool eventFilter(QObject*, QEvent*) noexcept override;
private:
	void deselect() const noexcept;
	bool isContainedBySelection(const QPoint& point) const noexcept;
	MyPlot* plot() const noexcept;
	void move(std::vector<MyPlotItem*>& items, const QPoint& pos) noexcept;
private slots:
	void select(const QRectF&) const noexcept;
};
