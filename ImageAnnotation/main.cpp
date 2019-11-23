#include "NImageAnnotationWidget.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include "NSelectClassLabelWidget.h"
#include <QDesktopWidget>
#include "NVocAnnationSerializer.h"

int main(int argc, char *argv[])
{

	QApplication::addLibraryPath("./plugins");
	QApplication a(argc, argv);
	QApplication::setWindowIcon(QIcon(":/ImageAnnotation/Resources/Images/application.png"));

	

	QFile file(":/ImageAnnotation/Resources/Main.css");
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		QString strQss = file.readAll();
		a.setStyleSheet(strQss);
	}
	NImageAnnotationWidget w;
	QRect screenRect = QApplication::desktop()->availableGeometry();
	w.resize(screenRect.width() * 0.85, screenRect.height() * 0.85);
	w.show();
// 	NVocAnnationSerializer vas;
// 	SImageAnnotationInfo info;
// 	vas.Read("annotationTest/2007_000713.xml", info);
// 	NSelectClassLabelWidget scw;
// 	scw.resize(200, 500);
// 	scw.show();
	return a.exec();
}
