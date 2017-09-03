#include "s0009d.h"
#include "GlWindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	S0009D w2;
	w2.show();
	return a.exec();
}
