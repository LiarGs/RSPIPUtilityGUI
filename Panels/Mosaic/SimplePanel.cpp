#include "SimplePanel.h"

namespace Panels::Mosaic {

bool SimplePanel::Run(const QString &globalSavePath) {
    QStringList imageFiles = _ImageSelector->Files();
    emit LogMessage(">> [Simple] 开始执行...");
    QApplication::processEvents();

    try {
        std::vector<RSPIP::GeoImage> images;
        images.reserve(imageFiles.count());

        emit LogMessage(">> 正在加载影像数据...");
        for (const QString &path : imageFiles) {
            auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
            if (imgPtr) {
                images.push_back(std::move(*imgPtr));
            } else {
                emit LogMessage("错误: 无法读取影像 " + path);
                return false;
            }
        }

        RSPIP::Algorithm::MosaicAlgorithm::Simple algorithm(images);
        emit LogMessage(">> 正在执行镶嵌...");
        QApplication::processEvents();
        algorithm.Execute();

        return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "Mosaic_Simple");

    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }
}

} // namespace Panels::Mosaic