#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

enum class MosaicPixelWiseOverlapStrategy {
    LastWriteWins,
    HighlightOverlapRed,
    MeanOfValidPixels,
    MedianOfValidPixels
};

struct MosaicPixelWiseStrategyRequest final : public MosaicRequest {
    MosaicPixelWiseOverlapStrategy Strategy = MosaicPixelWiseOverlapStrategy::LastWriteWins;
    QStringList MaskFiles;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct MosaicVoronoiPartitionRequest final : public MosaicRequest {
    QStringList MaskFiles;

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
