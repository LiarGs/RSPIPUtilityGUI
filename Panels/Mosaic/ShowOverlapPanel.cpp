#include "ShowOverlapPanel.h"

namespace Panels::Mosaic {

std::function<bool()> ShowOverlapPanel::BuildTask(const QString &globalSavePath) {
    const QStringList imageFiles = _ImageSelector->Files();

    return [this, imageFiles, globalSavePath]() {
        PostLog(">> [ShowOverLap] 开始执行...");

        try {
            std::vector<RSPIP::GeoImage> images;
            images.reserve(imageFiles.count());

            PostLog(">> 正在加载影像数据...");
            for (const QString &path : imageFiles) {
                auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
                if (imgPtr) {
                    images.push_back(std::move(*imgPtr));
                } else {
                    PostLog("错误: 无法读取影像 " + path);
                    return false;
                }
            }

            RSPIP::Algorithm::MosaicAlgorithm::ShowOverLap algorithm(images);
            PostLog(">> 正在计算重叠区域...");
            algorithm.Execute();

            return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "Mosaic_Overlap");

        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Mosaic


