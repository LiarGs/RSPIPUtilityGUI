#include "SimplePanel.h"
#include <QApplication>
// --- RSPIP Headers ---
#include "Algorithm/ImageReconstruct/Simple.h"
#include "Basic/CloudMask.h"
#include "IO/ImageReader.h"

namespace Panels::Reconstruct {

bool SimplePanel::Run(const QString &globalSavePath) {

    emit LogMessage(">> 正在加载影像数据...");
    QApplication::processEvents();

    auto targetImage = RSPIP::IO::GeoImageRead(_TargetSelect->CurrentPath().toStdString());
    auto referImage = RSPIP::IO::GeoImageRead(_ReferSelect->CurrentPath().toStdString());
    auto maskImage = RSPIP::IO::CloudMaskImageRead(_MaskSelect->CurrentPath().toStdString());

    if (!(targetImage && referImage && maskImage)) {
        emit LogMessage("错误: 无法读取影像或掩膜文件。");
        return false;
    }

    try {
        emit LogMessage(">> [Simple] 正在执行...");
        QApplication::processEvents();

        RSPIP::Algorithm::ReconstructAlgorithm::Simple algorithm(*targetImage, *referImage, *maskImage);
        algorithm.Execute();

        return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "ReconstructSimple");
    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }

    return false;
}

} // namespace Panels::Reconstruct
