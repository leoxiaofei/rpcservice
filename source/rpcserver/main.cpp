#include "testserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestServer w;
	w.show();
	return a.exec();
}
