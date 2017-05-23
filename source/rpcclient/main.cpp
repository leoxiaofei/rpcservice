#include "testclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestClient w;
	w.show();
	return a.exec();
}
