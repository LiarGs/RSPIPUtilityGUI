#pragma once

#include "Basic/Image.h"

#include <QString>
#include <QStringList>
#include <functional>
#include <optional>
#include <vector>

namespace Panels::Common {

using LogFn = std::function<void(const QString &)>;

struct BatchOutputPlan {
    QString OutputDir;
    QString OutputPrefix;
    bool SingleExactOutput = false;
};

std::optional<RSPIP::Image> ReadImage(const QString &path,
                                      const QString &role,
                                      const LogFn &log);

std::optional<std::vector<RSPIP::Image>> ReadImages(const QStringList &paths,
                                                    const QString &role,
                                                    const LogFn &log);

bool SaveImage(const RSPIP::Image &image,
               const QString &finalSavePath,
               const LogFn &log,
               const QString &successTemplate = QStringLiteral(">> 保存成功: %1"),
               const QString &failureTemplate = QStringLiteral("错误: 保存结果失败: %1"));

bool EnsureDirectory(const QString &directoryPath, const LogFn &log);

QString BuildAutoImageOutputPath(const QString &prefix,
                                 const QString &extension = QStringLiteral("tif"));

std::optional<BatchOutputPlan> ResolveBatchOutputPlan(const QString &requestedPath,
                                                      const QString &defaultPrefix,
                                                      int itemCount,
                                                      const LogFn &log);

std::optional<QString> ResolveStrictOutputDirectory(const QString &requestedPath,
                                                    const LogFn &log);

QString BuildOutputPath(const QString &outputDir, const QString &fileName);

QString BuildIndexedOutputFileName(const QString &prefix,
                                   int index,
                                   const QString &inputPath,
                                   const QString &extension = QStringLiteral("tif"));

QString BuildBasenameOutputFileName(const QString &prefix,
                                    const QString &inputPath,
                                    const QString &extension = QStringLiteral("tif"));

} // namespace Panels::Common
