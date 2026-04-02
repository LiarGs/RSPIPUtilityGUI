#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

struct PixelThresholdRequest final : public AlgorithmRequest {
    QStringList InputFiles;
    PixelThresholdMode Mode = PixelThresholdMode::Gray;
    int Threshold = 240;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

} // namespace Application::Execution
