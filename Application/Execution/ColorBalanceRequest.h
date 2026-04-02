#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

struct MatchStatisticsRequest final : public AlgorithmRequest {
    QString ReferencePath;
    QStringList InputFiles;
    QStringList MaskFiles;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

} // namespace Application::Execution
