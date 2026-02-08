#include "IsophotePanel.h"
#include <QApplication>
// --- RSPIP Headers ---
#include "Algorithm/ImageReconstruct/IsophoteConstrain.h"
#include "Basic/CloudMask.h"
#include "IO/ImageReader.h"

namespace Panels::Reconstruct {

bool IsophotePanel::Run(const QString &globalSavePath) {

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
        emit LogMessage(">> [IsophoteConstrain] 正在执行 (可能耗时较长)...");
        QApplication::processEvents();

        RSPIP::Algorithm::ReconstructAlgorithm::IsophoteConstrain algorithm(*targetImage, *referImage, *maskImage);
        algorithm.Execute();

        return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "ReconstructIsophote");
    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }
}

} // namespace Panels::Reconstruct
