#include <qobject.h>
#include <qrect.h>
#include <qwt_scale_widget.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
class QwtPlot;
//class QwtScaleWidget;

class ScalePicker : public QObject {
	Q_OBJECT
public:
	ScalePicker(QwtPlot* plot);
	virtual bool eventFilter(QObject*, QEvent*);

Q_SIGNALS:
	void clicked(int axis, double value);

private:
	void mouseClicked(const QwtScaleWidget*, const QPoint&);
	QRect scaleRect(const QwtScaleWidget*) const;
};
