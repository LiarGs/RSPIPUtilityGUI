#pragma once

#include "Application/Execution/ExecutionContext.h"

#include <QString>
#include <QStringList>

#include <optional>
#include <vector>

namespace Application::Execution {

enum class PixelThresholdMode {
    Gray,
    BlueBandOnly
};

enum class MaskSelectionMode {
    NonZeroSelected,
    ValueSetSelected
};

struct MaskSelectionPolicyRequest {
    int Band = 1;
    MaskSelectionMode Mode = MaskSelectionMode::NonZeroSelected;
    std::vector<int> SelectedValues;
};

struct AlgorithmRequest {
    virtual ~AlgorithmRequest() = default;

    QString SavePath;

    virtual Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const = 0;
};

struct SingleImageRequest : public AlgorithmRequest {
    QString TargetPath;
    QString ReferencePath;
    QString MaskPath;
};

struct MosaicRequest : public AlgorithmRequest {
    QStringList ImageFiles;
};

struct EvaluationRequest : public AlgorithmRequest {
    QString ImagePath;
    QString ReferencePath;
    QString MaskPath;
    bool BoundaryOnly = false;
    std::optional<MaskSelectionPolicyRequest> MaskPolicy = std::nullopt;
};

} // namespace Application::Execution
