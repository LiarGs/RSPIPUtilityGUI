#include "Application/Execution/ColorBalanceRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/ColorBalance/MatchStatistic.h"
#include "Basic/Image.h"
#include "Infrastructure/Execution/OutputPlanner.h"
#include "Infrastructure/IO/ImageIOService.h"

namespace Application::Execution {

using detail::BuildBatchSummary;
using detail::ExecuteSafely;

Infrastructure::Execution::ExecutionResult MatchStatisticsRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("MatchStatistics 执行失败"), context, [this, &context]() {
        context.Log.Post(QString(">> [MatchStatistics] 待处理影像数量: %1").arg(InputFiles.count()));
        context.Log.Post(">> 正在读取基准影像...");

        auto referenceImage = Infrastructure::IO::ReadImage(
            ReferencePath,
            QStringLiteral("基准影像"),
            context.Log);
        if (!referenceImage) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        const auto outputPlan = Infrastructure::Execution::ResolveBatchOutputPlan(
            SavePath,
            QStringLiteral("match_stat"),
            InputFiles.count(),
            context.Log);
        if (!outputPlan) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        int successCount = 0;
        int warningCount = 0;
        QStringList savedPaths;

        for (int i = 0; i < InputFiles.count(); ++i) {
            const QString &inputPath = InputFiles.at(i);
            const QString maskPath = i < MaskFiles.count() ? MaskFiles.at(i) : QString();

            context.Log.Post(QString(">> [%1/%2] 正在读取待匀色影像: %3")
                                 .arg(i + 1)
                                 .arg(InputFiles.count())
                                 .arg(inputPath));
            auto targetImage = Infrastructure::IO::ReadImage(
                inputPath,
                QStringLiteral("待匀色影像"),
                context.Log);
            if (!targetImage) {
                ++warningCount;
                continue;
            }

            RSPIP::Image maskImage;
            if (!maskPath.isEmpty()) {
                context.Log.Post(QString(">> [%1/%2] 正在读取掩膜: %3")
                                     .arg(i + 1)
                                     .arg(InputFiles.count())
                                     .arg(maskPath));
                auto loadedMask = Infrastructure::IO::ReadImage(
                    maskPath,
                    QStringLiteral("掩膜文件"),
                    context.Log);
                if (!loadedMask) {
                    ++warningCount;
                    continue;
                }
                maskImage = *loadedMask;
            }

            RSPIP::Algorithm::ColorBalanceAlgorithm::MatchStatistics algorithm(
                *targetImage,
                *referenceImage,
                maskImage);
            context.Log.Post(QString(">> [%1/%2] 正在执行匀色...")
                                 .arg(i + 1)
                                 .arg(InputFiles.count()));
            algorithm.Execute();

            const QString finalSavePath = outputPlan->SingleExactOutput
                                              ? SavePath.trimmed()
                                              : Infrastructure::Execution::BuildOutputPath(
                                                    outputPlan->OutputDir,
                                                    Infrastructure::Execution::BuildIndexedOutputFileName(
                                                        outputPlan->OutputPrefix,
                                                        i + 1,
                                                        inputPath));
            if (!Infrastructure::IO::SaveImage(
                    algorithm.AlgorithmResult,
                    finalSavePath,
                    context.Log,
                    QStringLiteral(">> 保存成功: %1"),
                    QStringLiteral("错误: 保存结果失败: %1"))) {
                ++warningCount;
                continue;
            }

            savedPaths.append(finalSavePath);
            ++successCount;
        }

        return BuildBatchSummary(QStringLiteral("MatchStatistics"),
                                 successCount,
                                 InputFiles.count(),
                                 warningCount,
                                 savedPaths);
    });
}

} // namespace Application::Execution
