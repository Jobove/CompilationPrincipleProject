#include <fstream>
#include <iostream>
#include <QApplication>
#include <QProcess>

#include "LexParser.h"
#include "widget.h"

using std::string;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    Widget w;
    w.show();

    return QApplication::exec();
}
