#include "ColorBalancePanel.h"
// --- RSPIP Headers ---
#include "Basic/CloudMask.h"
#include "IO/ImageReader.h"

namespace Panels::Reconstruct {

std::function<bool()> ColorBalancePanel::BuildTask(const QString &globalSavePath) {
    const QString targetPath = _TargetSelect->CurrentPath();
    const QString referPath = _ReferSelect->CurrentPath();
    const QString maskPath = _MaskSelect->CurrentPath();

    return [this, targetPath, referPath, maskPath, globalSavePath]() {
        PostLog(">> 正在加载影像数据...");

        auto targetImage = RSPIP::IO::GeoImageRead(targetPath.toStdString());
        auto referImage = RSPIP::IO::GeoImageRead(referPath.toStdString());
        auto maskImage = RSPIP::IO::CloudMaskImageRead(maskPath.toStdString());

        if (!(targetImage && referImage && maskImage)) {
            PostLog("错误: 无法读取影像或掩膜文件。");
            return false;
        }

        try {
            PostLog(">> [ColorBalance] 正在执行...");

            RSPIP::Algorithm::ReconstructAlgorithm::ColorBalanceReconstruct algorithm(*targetImage, *referImage, *maskImage);
            algorithm.Execute();

            return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "ReconstructColorBalance");
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Reconstruct


