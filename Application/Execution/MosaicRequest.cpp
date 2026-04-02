#include "Application/Execution/MosaicRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/Mosaic.h"
#include "Infrastructure/IO/ImageIOService.h"

namespace Application::Execution {

using detail::BuildMosaicResult;
using detail::ExecuteSafely;
using detail::SaveMosaicResult;

Infrastructure::Execution::ExecutionResult MosaicSimpleRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::Simple 执行失败"), context, [this, &context]() {
        context.Log.Post(">> [Simple] 开始执行...");
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        RSPIP::Algorithm::MosaicAlgorithm::Simple algorithm(*images);
        context.Log.Post(">> 正在执行镶嵌...");
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveMosaicResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("Mosaic_Simple"),
            context,
            &savedPaths);
        return BuildMosaicResult(success, QStringLiteral("Mosaic::Simple"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult MosaicShowOverlapRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::ShowOverlap 执行失败"), context, [this, &context]() {
        context.Log.Post(">> [ShowOverLap] 开始执行...");
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        RSPIP::Algorithm::MosaicAlgorithm::ShowOverLap algorithm(*images);
        context.Log.Post(">> 正在计算重叠区域...");
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveMosaicResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("Mosaic_Overlap"),
            context,
            &savedPaths);
        return BuildMosaicResult(success, QStringLiteral("Mosaic::ShowOverlap"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult MosaicAdaptivePatchRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::AdaptivePatch 执行失败"), context, [this, &context]() {
        context.Log.Post(">> [AdaptivePatch] 开始执行...");
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> 正在加载掩膜数据...");
        const auto masks = Infrastructure::IO::ReadImages(MaskFiles, QStringLiteral("掩膜影像"), context.Log);
        if (!masks) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        RSPIP::Algorithm::MosaicAlgorithm::AdaptivePatch algorithm(*images, *masks);
        algorithm.SetStripWidth(StripWidth);
        context.Log.Post(QString(">> 镶嵌参数: 条带宽度=%1").arg(StripWidth));
        context.Log.Post(">> 正在执行自适应补丁镶嵌 (耗时操作)...");
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveMosaicResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("Mosaic_AdaptivePatch"),
            context,
            &savedPaths);
        return BuildMosaicResult(success, QStringLiteral("Mosaic::AdaptivePatch"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult MosaicAdaptiveColorBalancePatchRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::AdaptiveColorBalancePatch 执行失败"), context, [this, &context]() {
        context.Log.Post(">> [AdaptiveColorBalancePatch] 开始执行...");
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> 正在加载掩膜数据...");
        const auto masks = Infrastructure::IO::ReadImages(MaskFiles, QStringLiteral("掩膜影像"), context.Log);
        if (!masks) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        RSPIP::Algorithm::MosaicAlgorithm::AdaptiveColorBalancePatch algorithm(*images, *masks);
        algorithm.SetStripWidth(StripWidth);
        context.Log.Post(QString(">> 镶嵌参数: 条带宽度=%1").arg(StripWidth));
        context.Log.Post(">> 正在执行自适应匀色补丁镶嵌 (耗时操作)...");
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveMosaicResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("Mosaic_AdaptiveColorBalance"),
            context,
            &savedPaths);
        return BuildMosaicResult(success, QStringLiteral("Mosaic::AdaptiveColorBalancePatch"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult MosaicAdaptiveIsophotePatchRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::AdaptiveIsophotePatch 执行失败"), context, [this, &context]() {
        context.Log.Post(">> [AdaptiveIsophotePatch] 开始执行...");
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> 正在加载掩膜数据...");
        const auto masks = Infrastructure::IO::ReadImages(MaskFiles, QStringLiteral("掩膜影像"), context.Log);
        if (!masks) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        RSPIP::Algorithm::MosaicAlgorithm::AdaptiveIsophotePatch algorithm(*images, *masks);
        algorithm.SetStripWidth(StripWidth);
        algorithm.SetMaxIterations(MaxIterations);
        algorithm.SetEpsilon(Epsilon);
        context.Log.Post(QString(">> 求解参数: 条带宽度=%1, 最大迭代次数=%2, 残差=%3")
                             .arg(StripWidth)
                             .arg(MaxIterations)
                             .arg(Epsilon, 0, 'g', 6));
        context.Log.Post(">> 正在执行等照度自适应补丁镶嵌 (耗时操作)...");
        algorithm.Execute();

        QStringList savedPaths;
        const bool success = SaveMosaicResult(
            algorithm.AlgorithmResult,
            SavePath,
            QStringLiteral("Mosaic_AdaptiveIsophote"),
            context,
            &savedPaths);
        return BuildMosaicResult(success, QStringLiteral("Mosaic::AdaptiveIsophotePatch"), savedPaths);
    });
}

} // namespace Application::Execution
