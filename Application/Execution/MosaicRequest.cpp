#include "Application/Execution/MosaicRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/Mosaic.h"
#include "Infrastructure/IO/ImageIOService.h"

namespace Application::Execution {

using detail::BuildMosaicResult;
using detail::ExecuteSafely;
using detail::SaveMosaicResult;

namespace {

QString PixelWiseStrategyLabel(MosaicPixelWiseOverlapStrategy strategy) {
    switch (strategy) {
    case MosaicPixelWiseOverlapStrategy::LastWriteWins:
        return QStringLiteral("LastWriteWins");
    case MosaicPixelWiseOverlapStrategy::HighlightOverlapRed:
        return QStringLiteral("HighlightOverlapRed");
    case MosaicPixelWiseOverlapStrategy::MeanOfValidPixels:
        return QStringLiteral("MeanOfValidPixels");
    case MosaicPixelWiseOverlapStrategy::MedianOfValidPixels:
        return QStringLiteral("MedianOfValidPixels");
    }

    return QStringLiteral("LastWriteWins");
}

QString PixelWiseOutputPrefix(MosaicPixelWiseOverlapStrategy strategy) {
    switch (strategy) {
    case MosaicPixelWiseOverlapStrategy::LastWriteWins:
        return QStringLiteral("Mosaic_PixelWise_LastWriteWins");
    case MosaicPixelWiseOverlapStrategy::HighlightOverlapRed:
        return QStringLiteral("Mosaic_PixelWise_HighlightOverlapRed");
    case MosaicPixelWiseOverlapStrategy::MeanOfValidPixels:
        return QStringLiteral("Mosaic_PixelWise_MeanOfValidPixels");
    case MosaicPixelWiseOverlapStrategy::MedianOfValidPixels:
        return QStringLiteral("Mosaic_PixelWise_MedianOfValidPixels");
    }

    return QStringLiteral("Mosaic_PixelWise_LastWriteWins");
}

RSPIP::Algorithm::MosaicAlgorithm::PixelWiseOverlapStrategy
ToCorePixelWiseStrategy(MosaicPixelWiseOverlapStrategy strategy) {
    switch (strategy) {
    case MosaicPixelWiseOverlapStrategy::LastWriteWins:
        return RSPIP::Algorithm::MosaicAlgorithm::PixelWiseOverlapStrategy::LastWriteWins;
    case MosaicPixelWiseOverlapStrategy::HighlightOverlapRed:
        return RSPIP::Algorithm::MosaicAlgorithm::PixelWiseOverlapStrategy::HighlightOverlapRed;
    case MosaicPixelWiseOverlapStrategy::MeanOfValidPixels:
        return RSPIP::Algorithm::MosaicAlgorithm::PixelWiseOverlapStrategy::MeanOfValidPixels;
    case MosaicPixelWiseOverlapStrategy::MedianOfValidPixels:
        return RSPIP::Algorithm::MosaicAlgorithm::PixelWiseOverlapStrategy::MedianOfValidPixels;
    }

    return RSPIP::Algorithm::MosaicAlgorithm::PixelWiseOverlapStrategy::LastWriteWins;
}

bool UsesAggregateMaskInputs(MosaicPixelWiseOverlapStrategy strategy) {
    return strategy == MosaicPixelWiseOverlapStrategy::MeanOfValidPixels ||
           strategy == MosaicPixelWiseOverlapStrategy::MedianOfValidPixels;
}

} // namespace

Infrastructure::Execution::ExecutionResult MosaicPixelWiseStrategyRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::PixelWiseStrategyMosaic 执行失败"), context, [this, &context]() {
        const QString strategyLabel = PixelWiseStrategyLabel(Strategy);
        context.Log.Post(QString(">> [PixelWiseStrategyMosaic] 开始执行，策略=%1").arg(strategyLabel));
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        QStringList savedPaths;
        bool success = false;

        if (UsesAggregateMaskInputs(Strategy) && !MaskFiles.isEmpty()) {
            if (MaskFiles.count() != ImageFiles.count()) {
                const QString message = QStringLiteral("MeanOfValidPixels / MedianOfValidPixels 要求掩膜数量与输入影像数量完全一致。");
                context.Log.Post(QStringLiteral("错误: %1").arg(message));
                Infrastructure::Execution::ExecutionResult result;
                result.Message = message;
                return result;
            }

            context.Log.Post(">> 正在加载掩膜数据...");
            const auto masks = Infrastructure::IO::ReadImages(MaskFiles, QStringLiteral("掩膜影像"), context.Log);
            if (!masks) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            RSPIP::Algorithm::MosaicAlgorithm::PixelWiseStrategyMosaic algorithm(*images, *masks);
            algorithm.SetOverlapStrategy(ToCorePixelWiseStrategy(Strategy));
            context.Log.Post(">> 正在执行像素替换策略镶嵌...");
            algorithm.Execute();

            success = SaveMosaicResult(
                algorithm.AlgorithmResult,
                SavePath,
                PixelWiseOutputPrefix(Strategy),
                context,
                &savedPaths);
        } else {
            RSPIP::Algorithm::MosaicAlgorithm::PixelWiseStrategyMosaic algorithm(*images);
            algorithm.SetOverlapStrategy(ToCorePixelWiseStrategy(Strategy));
            context.Log.Post(">> 正在执行像素替换策略镶嵌...");
            algorithm.Execute();

            success = SaveMosaicResult(
                algorithm.AlgorithmResult,
                SavePath,
                PixelWiseOutputPrefix(Strategy),
                context,
                &savedPaths);
        }

        return BuildMosaicResult(success, QStringLiteral("Mosaic::PixelWiseStrategyMosaic"), savedPaths);
    });
}

Infrastructure::Execution::ExecutionResult MosaicVoronoiPartitionRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("Mosaic::VoronoiPartitionMosaic 执行失败"), context, [this, &context]() {
        context.Log.Post(">> [VoronoiPartitionMosaic] 开始执行...");
        context.Log.Post(">> 正在加载影像数据...");
        const auto images = Infrastructure::IO::ReadImages(ImageFiles, QStringLiteral("输入影像"), context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        QStringList savedPaths;
        bool success = false;

        if (!MaskFiles.isEmpty()) {
            if (MaskFiles.count() != ImageFiles.count()) {
                const QString message = QStringLiteral("VoronoiPartitionMosaic 在提供掩膜时要求掩膜数量与输入影像数量完全一致。");
                context.Log.Post(QStringLiteral("错误: %1").arg(message));
                Infrastructure::Execution::ExecutionResult result;
                result.Message = message;
                return result;
            }

            context.Log.Post(">> 正在加载掩膜数据...");
            const auto masks = Infrastructure::IO::ReadImages(MaskFiles, QStringLiteral("掩膜影像"), context.Log);
            if (!masks) {
                return Infrastructure::Execution::ExecutionResult{};
            }

            RSPIP::Algorithm::MosaicAlgorithm::VoronoiPartitionMosaic algorithm(*images, *masks);
            context.Log.Post(">> 正在执行 Voronoi 分区镶嵌...");
            algorithm.Execute();

            success = SaveMosaicResult(
                algorithm.AlgorithmResult,
                SavePath,
                QStringLiteral("Mosaic_VoronoiPartition"),
                context,
                &savedPaths);
        } else {
            RSPIP::Algorithm::MosaicAlgorithm::VoronoiPartitionMosaic algorithm(*images);
            context.Log.Post(">> 正在执行 Voronoi 分区镶嵌...");
            algorithm.Execute();

            success = SaveMosaicResult(
                algorithm.AlgorithmResult,
                SavePath,
                QStringLiteral("Mosaic_VoronoiPartition"),
                context,
                &savedPaths);
        }

        return BuildMosaicResult(success, QStringLiteral("Mosaic::VoronoiPartitionMosaic"), savedPaths);
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
