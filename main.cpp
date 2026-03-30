#include "MainWindow.h"
#include "RSPIP.h"
#include "Util/SuperDebug.hpp"
#include "cpl_error.h"
#include <QApplication>
#include <opencv2/core.hpp>

namespace {

void CPL_STDCALL ForwardGdalMessage(CPLErr errClass, CPLErrorNum errNo, const char *msg) {
    const std::string detail = msg ? msg : "Unknown GDAL/PROJ message.";

    switch (errClass) {
    case CE_Debug:
        SuperDebug::Info("[GDAL:{}] {}", static_cast<int>(errNo), detail);
        break;
    case CE_Warning:
        SuperDebug::Warn("[GDAL:{}] {}", static_cast<int>(errNo), detail);
        break;
    case CE_Failure:
    case CE_Fatal:
        SuperDebug::Error("[GDAL:{}] {}", static_cast<int>(errNo), detail);
        break;
    case CE_None:
        break;
    }
}

int ForwardOpenCvError(int status, const char *funcName, const char *errMsg,
                       const char *fileName, int line, void *) {
    const std::string function = funcName ? funcName : "Unknown function";
    const std::string detail = errMsg ? errMsg : "Unknown OpenCV error.";
    const std::string file = fileName ? fileName : "Unknown file";

    SuperDebug::Error("[OpenCV:{}] {} ({} @ {}:{})",
                      status,
                      detail,
                      function,
                      file,
                      line);
    return 0;
}

} // namespace

int main(int argc, char *argv[]) {
    // 初始化 Qt 应用程序
    QApplication a(argc, argv);

    // 先创建主窗口，确保 SuperDebug 已经挂到 GUI 日志面板。
    UI::MainWindow mainWindow;

    CPLSetErrorHandler(ForwardGdalMessage);
    cv::redirectError(ForwardOpenCvError);
    GDALAllRegister();
    // =========================================================
    // 启动 GUI
    // =========================================================
    mainWindow.show();

    // 进入 Qt 事件循环
    int ret = a.exec();

    SuperDebug::Info("Application Exiting...");
    return ret;
}
