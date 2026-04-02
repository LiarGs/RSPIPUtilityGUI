#include "Application/Execution/ReconstructRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/Reconstruct.h"
#include "Infrastructure/IO/ImageIOService.h"
#include "Util/SuperDebug.hpp"

namespace Application::Execution {

using detail::BuildSingleImageResult;
using detail::ExecuteSafely;
using detail::SaveSingleImageResult;

Infrastructure::Execution::ExecutionResult ReconstructSimpleRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("SimpleReconstruct 执行失败"), context, [this, &context]() {
        context.Log.Post(">> 正在加载影像数据...");

        auto targetImage = Infrastructure::IO::ReadImage(TargetPath, QStringLiteral("目标影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
        if (!(targetImage && referenceImage && maskImage)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> [Simple] 正在执行...");
        RSPIP::Algorithm::ReconstructAlgorithm::Simple algorithm(*targetImage, *referenceImage, *maskImage);
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveSingleImageResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("ReconstructSimple"),
            context,
            &savedPaths);
        return BuildSingleImageResult(success, QStringLiteral("SimpleReconstruct"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult ReconstructColorBalanceRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("ColorBalanceReconstruct 执行失败"), context, [this, &context]() {
        context.Log.Post(">> 正在加载影像数据...");

        auto targetImage = Infrastructure::IO::ReadImage(TargetPath, QStringLiteral("目标影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
        if (!(targetImage && referenceImage && maskImage)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> [ColorBalance] 正在执行...");
        RSPIP::Algorithm::ReconstructAlgorithm::ColorBalanceReconstruct algorithm(*targetImage, *referenceImage, *maskImage);
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveSingleImageResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("ReconstructColorBalance"),
            context,
            &savedPaths);
        return BuildSingleImageResult(success, QStringLiteral("ColorBalanceReconstruct"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult ReconstructIsophoteRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("IsophoteConstrain 执行失败"), context, [this, &context]() {
        context.Log.Post(">> 正在加载影像数据...");

        auto targetImage = Infrastructure::IO::ReadImage(TargetPath, QStringLiteral("目标影像"), context.Log);
        auto referenceImage = Infrastructure::IO::ReadImage(ReferencePath, QStringLiteral("参考影像"), context.Log);
        auto maskImage = Infrastructure::IO::ReadImage(MaskPath, QStringLiteral("掩膜影像"), context.Log);
        if (!(targetImage && referenceImage && maskImage)) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> [IsophoteConstrain] 正在执行 (可能耗时较长)...");
        RSPIP::Algorithm::ReconstructAlgorithm::IsophoteConstrain algorithm(*targetImage, *referenceImage, *maskImage);
        algorithm.SetMaxIterations(MaxIterations);
        algorithm.SetEpsilon(Epsilon);
        SuperDebug::ScopeTimer algorithmTimer("Algorithm Execution");
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveSingleImageResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("ReconstructIsophote"),
            context,
            &savedPaths);
        return BuildSingleImageResult(success, QStringLiteral("IsophoteConstrain"), savedPaths);
    });
}

} // namespace Application::Execution
