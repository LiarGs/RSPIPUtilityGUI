#pragma once

#include "Basic/Image.h"
#include "Infrastructure/Execution/ExecutionTypes.h"

#include <QString>
#include <QStringList>

#include <optional>
#include <vector>

namespace Infrastructure::IO {

std::optional<RSPIP::Image> ReadImage(const QString &path,
                                      const QString &role,
                                      const Execution::ExecutionLogSink &log);

std::optional<std::vector<RSPIP::Image>> ReadImages(const QStringList &paths,
                                                    const QString &role,
                                                    const Execution::ExecutionLogSink &log);

bool SaveImage(const RSPIP::Image &image,
               const QString &finalSavePath,
               const Execution::ExecutionLogSink &log,
               const QString &successTemplate = QStringLiteral(">> 保存成功: %1"),
               const QString &failureTemplate = QStringLiteral("错误: 保存结果失败: %1"));

} // namespace Infrastructure::IO
