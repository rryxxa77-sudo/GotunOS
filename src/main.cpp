#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <unistd.h>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("GötünOS Installer");
    app.setApplicationVersion("2.0");

    // Partitioning, pacstrap and arch-chroot all need root. Warn instead of
    // failing halfway through the install.
    if (geteuid() != 0) {
        const auto answer = QMessageBox::warning(nullptr, "GötünOS Installer",
            "The installer is not running as root.\n\n"
            "You can look around, but partitioning and installing will fail. "
            "Start it with sudo to actually install.\n\nContinue anyway?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) return 1;
    }

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
