#pragma once

#include "Infrastructure/Execution/ExecutionTypes.h"

#include <QString>

#include <optional>

namespace Infrastructure::Execution {

struct OutputPlan {
    QString OutputDir;
    QString OutputPrefix;
    bool SingleExactOutput = false;
};

bool EnsureDirectory(const QString &directoryPath,
                     const ExecutionLogSink &log);

QString BuildAutoImageOutputPath(const QString &prefix,
                                 const QString &extension = QStringLiteral("tif"));

std::optional<OutputPlan> ResolveBatchOutputPlan(const QString &requestedPath,
                                                 const QString &defaultPrefix,
                                                 int itemCount,
                                                 const ExecutionLogSink &log);

std::optional<QString> ResolveStrictOutputDirectory(const QString &requestedPath,
                                                    const ExecutionLogSink &log);

std::optional<QString> ResolveTextOutputPath(const QString &requestedPath,
                                             const QString &defaultPrefix,
                                             const ExecutionLogSink &log);

bool SaveTextFile(const QString &content,
                  const QString &finalSavePath,
                  const ExecutionLogSink &log,
                  const QString &successTemplate = QStringLiteral(">> 评估结果已保存: %1"),
                  const QString &failureTemplate = QStringLiteral("错误: 无法写入结果文件: %1"));

QString BuildOutputPath(const QString &outputDir,
                        const QString &fileName);

QString BuildIndexedOutputFileName(const QString &prefix,
                                   int index,
                                   const QString &inputPath,
                                   const QString &extension = QStringLiteral("tif"));

QString BuildBasenameOutputFileName(const QString &prefix,
                                    const QString &inputPath,
                                    const QString &extension = QStringLiteral("tif"));

} // namespace Infrastructure::Execution
