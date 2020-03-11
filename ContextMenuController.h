#pragma once
#include <QObject>

#include <vector>

class MyPlot;
class MyPlotIntervalCurve;
class QEvent;
class QPoint;

class ContextMenuController : public QObject {
	Q_OBJECT
public:
	ContextMenuController(MyPlot* plot);
	virtual bool eventFilter(QObject*, QEvent*) noexcept;
private:
	MyPlot* plot() noexcept;
	const MyPlot* plot() const noexcept;
	void contextMenuSingle();
	void contextMenuMulit();
	void contextMenuEmpty();
};
