#include <QApplication>
#include <QDebug>
#include <thread>
#include <chrono>
#include "View.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	View w; 
	w.show();
	return a.exec();
}
