#pragma once
#include "CurvesPositioner.h"

#include <QObject>

#include <vector>

class MyPlot;
class MyPlotIntervalCurve;
class MyPlotItem;
class QEvent;
class QPoint;
class QPoint;

class CanvasMover : public QObject {
	Q_OBJECT
public:
	CanvasMover(MyPlot* plot);
	bool eventFilter(QObject*, QEvent*) noexcept override;
private:
	MyPlot* plot() const noexcept;
	void move(std::vector<MyPlotItem*>&, const QPoint&) const noexcept;
};
