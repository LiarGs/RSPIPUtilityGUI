#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"

namespace Application::Execution {

struct PSNREvaluationRequest final : public EvaluationRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct RMSEEvaluationRequest final : public EvaluationRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct SSIMEvaluationRequest final : public EvaluationRequest {
    double K1 = 0.01;
    double K2 = 0.03;

    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

struct BoundaryGradientEvaluationRequest final : public EvaluationRequest {
    Infrastructure::Execution::ExecutionResult Execute(const ExecutionContext &context) const override;
};

} // namespace Application::Execution
