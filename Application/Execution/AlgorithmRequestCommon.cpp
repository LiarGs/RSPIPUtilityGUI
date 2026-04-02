#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Basic/Image.h"
#include "Basic/MaskSelectionPolicy.h"
#include "Infrastructure/Execution/OutputPlanner.h"
#include "Infrastructure/IO/ImageIOService.h"

#include <exception>

namespace Application::Execution::detail {

using Infrastructure::Execution::ExecutionResult;

ExecutionResult ExecuteSafely(const QString &failurePrefix,
                              const ExecutionContext &context,
                              const std::function<ExecutionResult()> &runner) {
    try {
        return runner();
    } catch (const std::exception &e) {
        context.Log.Post(QString("异常: %1").arg(e.what()));
        ExecutionResult result;
        result.Success = false;
        result.Message = QString("%1: %2").arg(failurePrefix, e.what());
        return result;
    }
}

ExecutionResult BuildBatchSummary(const QString &label,
                                  int successCount,
                                  int totalCount,
                                  int warningCount,
                                  const QStringList &savedPaths) {
    ExecutionResult result;
    result.Success = successCount > 0;
    result.WarningCount = warningCount;
    result.SavedPaths = savedPaths;
    result.Message = QString("%1 完成，成功 %2 / %3。")
                         .arg(label)
                         .arg(successCount)
                         .arg(totalCount);
    return result;
}

bool SaveSingleImageResult(const RSPIP::Image &image,
                           const QString &userPath,
                           const QString &prefix,
                           const ExecutionContext &context,
                           QStringList *savedPaths) {
    QString finalSavePath = userPath.trimmed();
    if (finalSavePath.isEmpty()) {
        finalSavePath = Infrastructure::Execution::BuildAutoImageOutputPath(prefix);
        context.Log.Post(">> 使用自动生成的路径: " + finalSavePath);
    }

    if (!Infrastructure::IO::SaveImage(
            image,
            finalSavePath,
            context.Log,
            QStringLiteral(">> 结果已保存: %1"),
            QStringLiteral("错误: 结果保存失败: %1"))) {
        return false;
    }

    if (savedPaths) {
        savedPaths->append(finalSavePath);
    }
    return true;
}

ExecutionResult BuildSingleImageResult(bool success,
                                       const QString &label,
                                       const QStringList &savedPaths) {
    ExecutionResult result;
    result.Success = success;
    result.SavedPaths = savedPaths;
    result.Message = success
                         ? QString("%1 执行完成。").arg(label)
                         : QString("%1 执行失败。").arg(label);
    return result;
}

bool SaveMosaicResult(const RSPIP::Image &image,
                      const QString &userPath,
                      const QString &prefix,
                      const ExecutionContext &context,
                      QStringList *savedPaths) {
    QString finalSavePath = userPath.trimmed();
    if (finalSavePath.isEmpty()) {
        finalSavePath = Infrastructure::Execution::BuildAutoImageOutputPath(prefix);
        context.Log.Post(">> 使用自动生成的路径: " + finalSavePath);
    }

    if (!Infrastructure::IO::SaveImage(
            image,
            finalSavePath,
            context.Log,
            QStringLiteral(">> 镶嵌完成并保存: %1"),
            QStringLiteral("错误: 结果保存失败: %1"))) {
        return false;
    }

    if (savedPaths) {
        savedPaths->append(finalSavePath);
    }
    return true;
}

ExecutionResult BuildMosaicResult(bool success,
                                  const QString &label,
                                  const QStringList &savedPaths) {
    ExecutionResult result;
    result.Success = success;
    result.SavedPaths = savedPaths;
    result.Message = success
                         ? QString("%1 执行完成。").arg(label)
                         : QString("%1 执行失败。").arg(label);
    return result;
}

bool ValidatePairwiseCompatible(const RSPIP::Image &imageData,
                                const RSPIP::Image &referenceImage,
                                const ExecutionContext &context) {
    if (imageData.Width() != referenceImage.Width() ||
        imageData.Height() != referenceImage.Height()) {
        context.Log.Post("错误: 两幅影像的宽高不一致，无法进行指标评估。");
        return false;
    }

    return true;
}

bool ValidateMaskCompatible(const RSPIP::Image &imageData,
                            const RSPIP::Image &maskImage,
                            const ExecutionContext &context) {
    if (imageData.Width() != maskImage.Width() ||
        imageData.Height() != maskImage.Height()) {
        context.Log.Post("错误: 掩膜与待评估影像的宽高不一致，无法进行指标评估。");
        return false;
    }

    return true;
}

QString DescribeMaskSelectionPolicy(const RSPIP::MaskSelectionPolicy &policy) {
    QString description = QString("Band=%1, Mode=%2")
                              .arg(policy.Band)
                              .arg(policy.Mode == RSPIP::MaskSelectionMode::NonZeroSelected
                                       ? "NonZeroSelected"
                                       : "ValueSetSelected");
    if (policy.Mode == RSPIP::MaskSelectionMode::ValueSetSelected) {
        QStringList values;
        for (int value : policy.SelectedValues) {
            values << QString::number(value);
        }
        description += QString(", SelectedValues=[%1]").arg(values.join(", "));
    }
    return description;
}

bool BuildMaskSelectionPolicy(const RSPIP::Image &maskImage,
                              const std::optional<MaskSelectionPolicyRequest> &request,
                              RSPIP::MaskSelectionPolicy &policy,
                              QString *description,
                              const ExecutionContext &context) {
    policy = {};
    if (!request) {
        policy.Band = 1;
        policy.Mode = RSPIP::MaskSelectionMode::NonZeroSelected;
        if (description) {
            *description = QStringLiteral("Band=1, Mode=NonZeroSelected");
        }
        return true;
    }

    policy.Band = request->Band;
    policy.Mode = request->Mode == MaskSelectionMode::ValueSetSelected
                      ? RSPIP::MaskSelectionMode::ValueSetSelected
                      : RSPIP::MaskSelectionMode::NonZeroSelected;

    if (maskImage.GetBandCounts() <= 0 || policy.Band > maskImage.GetBandCounts()) {
        context.Log.Post(QString("错误: 掩膜 Band=%1 超出范围，当前掩膜只有 %2 个波段。")
                             .arg(policy.Band)
                             .arg(maskImage.GetBandCounts()));
        return false;
    }

    if (policy.Mode == RSPIP::MaskSelectionMode::ValueSetSelected) {
        if (request->SelectedValues.empty()) {
            context.Log.Post("错误: ValueSetSelected 模式下必须提供 SelectedValues。");
            return false;
        }
        policy.SelectedValues = request->SelectedValues;
    }

    if (description) {
        *description = DescribeMaskSelectionPolicy(policy);
    }
    return true;
}

bool SaveEvaluationText(const QString &content,
                        const QString &userPath,
                        const QString &prefix,
                        const ExecutionContext &context,
                        QStringList *savedPaths) {
    const auto finalSavePath = Infrastructure::Execution::ResolveTextOutputPath(userPath, prefix, context.Log);
    if (!finalSavePath) {
        return false;
    }

    if (!Infrastructure::Execution::SaveTextFile(content, *finalSavePath, context.Log)) {
        return false;
    }

    if (savedPaths) {
        savedPaths->append(*finalSavePath);
    }
    return true;
}

ExecutionResult BuildEvaluationResult(bool success,
                                      const QString &metricName,
                                      const QStringList &savedPaths,
                                      int warningCount) {
    ExecutionResult result;
    result.Success = success;
    result.SavedPaths = savedPaths;
    result.WarningCount = warningCount;
    result.Message = success
                         ? QString("%1 评估完成。").arg(metricName)
                         : QString("%1 评估失败。").arg(metricName);
    return result;
}

} // namespace Application::Execution::detail
