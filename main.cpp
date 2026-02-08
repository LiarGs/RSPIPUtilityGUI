#include "MainWindow.h"
#include "RSPIP.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // 初始化 Qt 应用程序
    QApplication a(argc, argv);
    GDALAllRegister();
    // =========================================================
    // 启动 GUI
    // =========================================================
    UI::MainWindow mainWindow;
    mainWindow.show();

    // 进入 Qt 事件循环
    int ret = a.exec();

    SuperDebug::Info("Application Exiting...");
    return ret;
}
