#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("GötünOS Installer");
    app.setApplicationVersion("2.0");

    // Load QSS theme
    for (const QString &path : {":style.qss", ":/style.qss", "assets/style.qss"}) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            app.setStyleSheet(in.readAll());
            break;
        }
    }

    MainWindow w;
    w.show();
    return app.exec();
}
