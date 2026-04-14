#include <QApplication>
#include <QStandardPaths>
#include "windows/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("TradeNote");
    app.setApplicationName("TradeNote");

    MainWindow window;
    window.show();
    
    return app.exec();
}
