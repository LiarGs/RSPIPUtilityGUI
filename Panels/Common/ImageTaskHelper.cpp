#include "ImageTaskHelper.h"

#include "Infrastructure/Execution/OutputPlanner.h"
#include "Infrastructure/IO/ImageIOService.h"

namespace Panels::Common {

std::optional<RSPIP::Image> ReadImage(const QString &path,
                                      const QString &role,
                                      const LogFn &log) {
    return Infrastructure::IO::ReadImage(path, role, Infrastructure::Execution::ExecutionLogSink(log));
}

std::optional<std::vector<RSPIP::Image>> ReadImages(const QStringList &paths,
                                                    const QString &role,
                                                    const LogFn &log) {
    return Infrastructure::IO::ReadImages(paths, role, Infrastructure::Execution::ExecutionLogSink(log));
}

bool SaveImage(const RSPIP::Image &image,
               const QString &finalSavePath,
               const LogFn &log,
               const QString &successTemplate,
               const QString &failureTemplate) {
    return Infrastructure::IO::SaveImage(image,
                                         finalSavePath,
                                         Infrastructure::Execution::ExecutionLogSink(log),
                                         successTemplate,
                                         failureTemplate);
}

bool EnsureDirectory(const QString &directoryPath, const LogFn &log) {
    return Infrastructure::Execution::EnsureDirectory(directoryPath,
                                                      Infrastructure::Execution::ExecutionLogSink(log));
}

QString BuildAutoImageOutputPath(const QString &prefix, const QString &extension) {
    return Infrastructure::Execution::BuildAutoImageOutputPath(prefix, extension);
}

std::optional<BatchOutputPlan> ResolveBatchOutputPlan(const QString &requestedPath,
                                                      const QString &defaultPrefix,
                                                      int itemCount,
                                                      const LogFn &log) {
    const auto result = Infrastructure::Execution::ResolveBatchOutputPlan(
        requestedPath,
        defaultPrefix,
        itemCount,
        Infrastructure::Execution::ExecutionLogSink(log));
    if (!result) {
        return std::nullopt;
    }

    return BatchOutputPlan{
        result->OutputDir,
        result->OutputPrefix,
        result->SingleExactOutput
    };
}

std::optional<QString> ResolveStrictOutputDirectory(const QString &requestedPath,
                                                    const LogFn &log) {
    return Infrastructure::Execution::ResolveStrictOutputDirectory(
        requestedPath,
        Infrastructure::Execution::ExecutionLogSink(log));
}

QString BuildOutputPath(const QString &outputDir, const QString &fileName) {
    return Infrastructure::Execution::BuildOutputPath(outputDir, fileName);
}

QString BuildIndexedOutputFileName(const QString &prefix,
                                   int index,
                                   const QString &inputPath,
                                   const QString &extension) {
    return Infrastructure::Execution::BuildIndexedOutputFileName(prefix, index, inputPath, extension);
}

QString BuildBasenameOutputFileName(const QString &prefix,
                                    const QString &inputPath,
                                    const QString &extension) {
    return Infrastructure::Execution::BuildBasenameOutputFileName(prefix, inputPath, extension);
}

} // namespace Panels::Common
