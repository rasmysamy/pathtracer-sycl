#include <CL/sycl.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "mainwindow.h"
#include <fstream>

using material::MATERIALS;

int main(int argc, char *argv[]) {
    std::ofstream output("/tmp/mptt.json");
    output.close();
    QApplication a(argc, argv);

    MainWindow w;
    w.setGeometry(
            QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    w.size(),
                    qApp->desktop()->screenGeometry()
            )
    );

    w.show();
    w.setColorLabelsInitial();

    return QApplication::exec();
}
