#include "Application/Execution/EvaluationRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/Evaluation/BoundaryGradientEvaluator.h"
#include "Algorithm/Evaluation/PSNREvaluator.h"
#include "Algorithm/Evaluation/RMSEEvaluator.h"
#include "Algorithm/Evaluation/SSIMEvaluator.h"
#include "Basic/MaskSelectionPolicy.h"
#include "Infrastructure/IO/ImageIOService.h"

#include <QDateTime>
#include <QTextStream>

namespace Application::Execution {

using detail::BuildEvaluationResult;
using detail::BuildMaskSelectionPolicy;
using detail::ExecuteSafely;
using detail::SaveEvaluationText;
using detail::ValidateMaskCompatible;
using detail::ValidatePairwiseCompatible;

Infrastructure::Execution::ExecutionResult PSNREvaluationRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("PSNR 评估失败"), context, [this, &context]() {
        context.Log.Post(">> [PSNREvaluator] 正在读取影像数据...");

        auto imageData = Infrastructure::IO::ReadImage(ImagePath, QStringLiteral("待评估影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        if (!imageData || !referenceImage) {
            return Infrastructure::Execution::ExecutionResult{};
        }
        if (!ValidatePairwiseCompatible(*imageData, *referenceImage, context)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        QString maskPolicyDescription = QStringLiteral("<none>");
        double value = 0.0;

        if (!MaskPath.isEmpty()) {
            context.Log.Post(">> [PSNREvaluator] 正在读取掩膜数据...");
            auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
            if (!maskImage || !ValidateMaskCompatible(*imageData, *maskImage, context)) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            RSPIP::MaskSelectionPolicy policy = {};
            if (!BuildMaskSelectionPolicy(*maskImage, MaskPolicy, policy, &maskPolicyDescription, context)) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            context.Log.Post(">> [PSNREvaluator] 正在计算 PSNR...");
            RSPIP::Algorithm::PSNREvaluator algorithm(*imageData, *referenceImage, *maskImage);
            algorithm.SetMaskSelectionPolicy(policy);
            algorithm.Execute();
            value = algorithm.EvaluateResult;
        } else {
            context.Log.Post(">> [PSNREvaluator] 正在计算 PSNR...");
            RSPIP::Algorithm::PSNREvaluator algorithm(*imageData, *referenceImage);
            algorithm.Execute();
            value = algorithm.EvaluateResult;
        }

        context.Log.Post(QString(">> PSNR = %1 dB").arg(value, 0, 'f', 6));

        QString content;
        QTextStream stream(&content);
        stream << "Metric: PSNR\n";
        stream << "Value: " << QString::number(value, 'f', 6) << " dB\n";
        stream << "Image: " << ImagePath << "\n";
        stream << "Reference: " << ReferencePath << "\n";
        stream << "Mask: " << (MaskPath.isEmpty() ? "<none>" : MaskPath) << "\n";
        stream << "MaskPolicy: " << maskPolicyDescription << "\n";
        stream << "GeneratedAt: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";

        QStringList savedPaths;
        const bool success = SaveEvaluationText(content, SavePath, QStringLiteral("psnr_eval"), context, &savedPaths);
        return BuildEvaluationResult(success, QStringLiteral("PSNR"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult RMSEEvaluationRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("RMSE 评估失败"), context, [this, &context]() {
        context.Log.Post(">> [RMSEEvaluator] 正在读取影像数据...");

        auto imageData = Infrastructure::IO::ReadImage(ImagePath, QStringLiteral("待评估影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        if (!imageData || !referenceImage) {
            return Infrastructure::Execution::ExecutionResult{};
        }
        if (!ValidatePairwiseCompatible(*imageData, *referenceImage, context)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        QString maskPolicyDescription = QStringLiteral("<none>");
        double value = 0.0;

        if (!MaskPath.isEmpty()) {
            context.Log.Post(">> [RMSEEvaluator] 正在读取掩膜数据...");
            auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
            if (!maskImage || !ValidateMaskCompatible(*imageData, *maskImage, context)) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            RSPIP::MaskSelectionPolicy policy = {};
            if (!BuildMaskSelectionPolicy(*maskImage, MaskPolicy, policy, &maskPolicyDescription, context)) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            context.Log.Post(">> [RMSEEvaluator] 正在计算 RMSE...");
            RSPIP::Algorithm::RMSEEvaluator algorithm(*imageData, *referenceImage, *maskImage);
            algorithm.SetBoundaryOnly(BoundaryOnly);
            algorithm.SetMaskSelectionPolicy(policy);
            algorithm.Execute();
            value = algorithm.EvaluateResult;
        } else {
            context.Log.Post(">> [RMSEEvaluator] 正在计算 RMSE...");
            RSPIP::Algorithm::RMSEEvaluator algorithm(*imageData, *referenceImage);
            algorithm.Execute();
            value = algorithm.EvaluateResult;
        }

        context.Log.Post(QString(">> RMSE = %1").arg(value, 0, 'f', 6));

        QString content;
        QTextStream stream(&content);
        stream << "Metric: RMSE\n";
        stream << "Value: " << QString::number(value, 'f', 6) << "\n";
        stream << "Image: " << ImagePath << "\n";
        stream << "Reference: " << ReferencePath << "\n";
        stream << "Mask: " << (MaskPath.isEmpty() ? "<none>" : MaskPath) << "\n";
        stream << "MaskPolicy: " << maskPolicyDescription << "\n";
        stream << "BoundaryOnly: " << (BoundaryOnly ? "true" : "false") << "\n";
        stream << "GeneratedAt: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";

        QStringList savedPaths;
        const bool success = SaveEvaluationText(content, SavePath, QStringLiteral("rmse_eval"), context, &savedPaths);
        return BuildEvaluationResult(success, QStringLiteral("RMSE"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult SSIMEvaluationRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("SSIM 评估失败"), context, [this, &context]() {
        context.Log.Post(">> [SSIMEvaluator] 正在读取影像数据...");

        auto imageData = Infrastructure::IO::ReadImage(ImagePath, QStringLiteral("待评估影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        if (!imageData || !referenceImage) {
            return Infrastructure::Execution::ExecutionResult{};
        }
        if (!ValidatePairwiseCompatible(*imageData, *referenceImage, context)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        QString maskPolicyDescription = QStringLiteral("<none>");
        double value = 0.0;

        if (!MaskPath.isEmpty()) {
            context.Log.Post(">> [SSIMEvaluator] 正在读取掩膜数据...");
            auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
            if (!maskImage || !ValidateMaskCompatible(*imageData, *maskImage, context)) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            RSPIP::MaskSelectionPolicy policy = {};
            if (!BuildMaskSelectionPolicy(*maskImage, MaskPolicy, policy, &maskPolicyDescription, context)) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            context.Log.Post(">> [SSIMEvaluator] 正在计算 SSIM...");
            RSPIP::Algorithm::SSIMEvaluator algorithm(*imageData, *referenceImage, *maskImage);
            algorithm.SetK1(K1);
            algorithm.SetK2(K2);
            algorithm.SetBoundaryOnly(BoundaryOnly);
            algorithm.SetMaskSelectionPolicy(policy);
            algorithm.Execute();
            value = algorithm.EvaluateResult;
        } else {
            context.Log.Post(">> [SSIMEvaluator] 正在计算 SSIM...");
            RSPIP::Algorithm::SSIMEvaluator algorithm(*imageData, *referenceImage);
            algorithm.SetK1(K1);
            algorithm.SetK2(K2);
            algorithm.Execute();
            value = algorithm.EvaluateResult;
        }

        context.Log.Post(QString(">> SSIM = %1").arg(value, 0, 'f', 6));

        QString content;
        QTextStream stream(&content);
        stream << "Metric: SSIM\n";
        stream << "Value: " << QString::number(value, 'f', 6) << "\n";
        stream << "K1: " << QString::number(K1, 'f', 4) << "\n";
        stream << "K2: " << QString::number(K2, 'f', 4) << "\n";
        stream << "Image: " << ImagePath << "\n";
        stream << "Reference: " << ReferencePath << "\n";
        stream << "Mask: " << (MaskPath.isEmpty() ? "<none>" : MaskPath) << "\n";
        stream << "MaskPolicy: " << maskPolicyDescription << "\n";
        stream << "BoundaryOnly: " << (BoundaryOnly ? "true" : "false") << "\n";
        stream << "GeneratedAt: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";

        QStringList savedPaths;
        const bool success = SaveEvaluationText(content, SavePath, QStringLiteral("ssim_eval"), context, &savedPaths);
        return BuildEvaluationResult(success, QStringLiteral("SSIM"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult BoundaryGradientEvaluationRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("BoundaryGradient 评估失败"), context, [this, &context]() {
        context.Log.Post(">> [BoundaryGradientEvaluator] 正在读取影像数据...");

        auto imageData = Infrastructure::IO::ReadImage(ImagePath, QStringLiteral("待评估影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
        if (!imageData || !referenceImage || !maskImage) {
            return Infrastructure::Execution::ExecutionResult{};
        }
        if (!ValidatePairwiseCompatible(*imageData, *referenceImage, context) ||
            !ValidateMaskCompatible(*imageData, *maskImage, context)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        RSPIP::MaskSelectionPolicy policy = {};
        QString maskPolicyDescription;
        if (!BuildMaskSelectionPolicy(*maskImage, MaskPolicy, policy, &maskPolicyDescription, context)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> [BoundaryGradientEvaluator] 正在计算边界梯度...");
        RSPIP::Algorithm::BoundaryGradientEvaluator algorithm(*imageData, *referenceImage, *maskImage);
        algorithm.SetMaskSelectionPolicy(policy);
        algorithm.Execute();
        const double value = algorithm.EvaluateResult;

        context.Log.Post(QString(">> BoundaryGradient = %1").arg(value, 0, 'f', 6));

        QString content;
        QTextStream stream(&content);
        stream << "Metric: BoundaryGradient\n";
        stream << "Value: " << QString::number(value, 'f', 6) << "\n";
        stream << "Image: " << ImagePath << "\n";
        stream << "Reference: " << ReferencePath << "\n";
        stream << "Mask: " << MaskPath << "\n";
        stream << "MaskPolicy: " << maskPolicyDescription << "\n";
        stream << "GeneratedAt: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";

        QStringList savedPaths;
        const bool success = SaveEvaluationText(content, SavePath, QStringLiteral("boundary_gradient_eval"), context, &savedPaths);
        return BuildEvaluationResult(success, QStringLiteral("BoundaryGradient"), savedPaths);
    });
}

} // namespace Application::Execution
