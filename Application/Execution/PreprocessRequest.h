#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

struct GeoCoordinateAlignRequest final : public AlgorithmRequest {
    QStringList ImageFiles;
    QStringList MaskFiles;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

} // namespace Application::Execution
