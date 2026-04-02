#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

struct MosaicSimpleRequest final : public MosaicRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct MosaicShowOverlapRequest final : public MosaicRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct MosaicAdaptivePatchRequest final : public MosaicRequest {
    QStringList MaskFiles;
    int StripWidth = 32;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct MosaicAdaptiveColorBalancePatchRequest final : public MosaicRequest {
    QStringList MaskFiles;
    int StripWidth = 32;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct MosaicAdaptiveIsophotePatchRequest final : public MosaicRequest {
    QStringList MaskFiles;
    int StripWidth = 32;
    int MaxIterations = 10000;
    double Epsilon = 1.0;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

} // namespace Application::Execution
