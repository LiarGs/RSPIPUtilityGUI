#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

struct ReconstructSimpleRequest final : public SingleImageRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct ReconstructColorBalanceRequest final : public SingleImageRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct ReconstructIsophoteRequest final : public SingleImageRequest {
    int MaxIterations = 1000;
    double Epsilon = 1.0;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

} // namespace Application::Execution
