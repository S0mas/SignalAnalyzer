#pragma once
#include <QObject>

class MyPlot;
class MyPlotIntervalCurve;
class MyPlotPicker;
class QEvent;
class QPoint;
class QRectF;

class CanvasSelector : public QObject {
	Q_OBJECT
	MyPlotPicker* picker_;
public:
	CanvasSelector(MyPlot* plot);
	bool eventFilter(QObject*, QEvent*) noexcept override;
private:
	void deselect() const noexcept;
	bool isContainedBySelection(const QPoint& point) const noexcept;
	MyPlot* plot() const noexcept;
private slots:
	void select(const QRectF&) const noexcept;
};
