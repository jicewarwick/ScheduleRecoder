#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QTranslator translator;
	//    translator.load("translations/main_" + QLocale::system().name());
	translator.load("translations/main_zh_CN");
	a.installTranslator(&translator);
	MainWindow w;
	return a.exec();
}
