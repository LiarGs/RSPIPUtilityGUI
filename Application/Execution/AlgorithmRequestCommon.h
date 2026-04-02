#pragma once

#include "Application/Execution/AlgorithmRequestBase.h"
#include "Infrastructure/Execution/ExecutionTypes.h"

#include <functional>

namespace RSPIP {
class Image;
class MaskSelectionPolicy;
} // namespace RSPIP

namespace Application::Execution::detail {

using Infrastructure::Execution::ExecutionResult;

ExecutionResult ExecuteSafely(const QString &failurePrefix,
                              const ExecutionContext &context,
                              const std::function<ExecutionResult()> &runner);

ExecutionResult BuildBatchSummary(const QString &label,
                                  int successCount,
                                  int totalCount,
                                  int warningCount,
                                  const QStringList &savedPaths);

bool SaveSingleImageResult(const RSPIP::Image &image,
                           const QString &userPath,
                           const QString &prefix,
                           const ExecutionContext &context,
                           QStringList *savedPaths);

ExecutionResult BuildSingleImageResult(bool success,
                                       const QString &label,
                                       const QStringList &savedPaths);

bool SaveMosaicResult(const RSPIP::Image &image,
                      const QString &userPath,
                      const QString &prefix,
                      const ExecutionContext &context,
                      QStringList *savedPaths);

ExecutionResult BuildMosaicResult(bool success,
                                  const QString &label,
                                  const QStringList &savedPaths);

bool ValidatePairwiseCompatible(const RSPIP::Image &imageData,
                                const RSPIP::Image &referenceImage,
                                const ExecutionContext &context);

bool ValidateMaskCompatible(const RSPIP::Image &imageData,
                            const RSPIP::Image &maskImage,
                            const ExecutionContext &context);

QString DescribeMaskSelectionPolicy(const RSPIP::MaskSelectionPolicy &policy);

bool BuildMaskSelectionPolicy(const RSPIP::Image &maskImage,
                              const std::optional<MaskSelectionPolicyRequest> &request,
                              RSPIP::MaskSelectionPolicy &policy,
                              QString *description,
                              const ExecutionContext &context);

bool SaveEvaluationText(const QString &content,
                        const QString &userPath,
                        const QString &prefix,
                        const ExecutionContext &context,
                        QStringList *savedPaths);

ExecutionResult BuildEvaluationResult(bool success,
                                      const QString &metricName,
                                      const QStringList &savedPaths,
                                      int warningCount = 0);

} // namespace Application::Execution::detail
