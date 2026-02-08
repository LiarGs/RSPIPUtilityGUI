#include "ShowOverlapPanel.h"

namespace Panels::Mosaic {

bool ShowOverlapPanel::Run(const QString &globalSavePath) {
    QStringList imageFiles = _ImageSelector->Files();
    emit LogMessage(">> [ShowOverLap] 开始执行...");
    QApplication::processEvents();

    try {
        std::vector<RSPIP::GeoImage> images;
        images.reserve(imageFiles.count());

        emit LogMessage(">> 正在加载影像数据...");
        for (const QString &path : imageFiles) {
            auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
            if (imgPtr)
                images.push_back(std::move(*imgPtr));
        }

        RSPIP::Algorithm::MosaicAlgorithm::ShowOverLap algorithm(images);
        emit LogMessage(">> 正在计算重叠区域...");
        QApplication::processEvents();
        algorithm.Execute();

        return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "Mosaic_Overlap");

    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }
}

} // namespace Panels::Mosaic
