#include "Infrastructure/IO/ImageIOService.h"

#include "IO/ImageReader.h"
#include "IO/ImageWriter.h"
#include "Infrastructure/Execution/OutputPlanner.h"

#include <QFileInfo>

namespace Infrastructure::IO {

std::optional<RSPIP::Image> ReadImage(const QString &path,
                                      const QString &role,
                                      const Execution::ExecutionLogSink &log) {
    const RSPIP::Image image = RSPIP::IO::ReadImage(path.toStdString());
    if (!image.ImageData.empty()) {
        return image;
    }

    log.Post(QString("错误: 无法读取%1: %2").arg(role, path));
    return std::nullopt;
}

std::optional<std::vector<RSPIP::Image>> ReadImages(const QStringList &paths,
                                                    const QString &role,
                                                    const Execution::ExecutionLogSink &log) {
    std::vector<RSPIP::Image> images;
    images.reserve(paths.size());

    for (const QString &path : paths) {
        auto image = ReadImage(path, role, log);
        if (!image) {
            return std::nullopt;
        }
        images.push_back(std::move(*image));
    }

    return images;
}

bool SaveImage(const RSPIP::Image &image,
               const QString &finalSavePath,
               const Execution::ExecutionLogSink &log,
               const QString &successTemplate,
               const QString &failureTemplate) {
    if (finalSavePath.trimmed().isEmpty()) {
        log.Post(QStringLiteral("错误: 输出路径为空。"));
        return false;
    }

    const QFileInfo outputInfo(finalSavePath);
    const QString outputDir = outputInfo.absolutePath();
    if (!Execution::EnsureDirectory(outputDir, log)) {
        return false;
    }

    const bool saved = RSPIP::IO::SaveImage(image,
                                            outputDir.toStdString(),
                                            outputInfo.fileName().toStdString());
    if (!saved) {
        log.Post(failureTemplate.arg(finalSavePath));
        return false;
    }

    log.Post(successTemplate.arg(finalSavePath));
    return true;
}

} // namespace Infrastructure::IO
