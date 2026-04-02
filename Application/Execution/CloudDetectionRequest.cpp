#include "Application/Execution/CloudDetectionRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/CloudDetection/PixelThreshold.h"
#include "Infrastructure/Execution/OutputPlanner.h"
#include "Infrastructure/IO/ImageIOService.h"

namespace Application::Execution {

using detail::BuildBatchSummary;
using detail::ExecuteSafely;

Infrastructure::Execution::ExecutionResult PixelThresholdRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("CloudDetection::PixelThreshold 执行失败"), context, [this, &context]() {
        context.Log.Post(QString(">> [CloudDetection::PixelThreshold] 待处理影像数量: %1")
                             .arg(InputFiles.count()));

        const auto outputPlan = Infrastructure::Execution::ResolveBatchOutputPlan(
            SavePath,
            QStringLiteral("cloud_mask"),
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
            context.Log.Post(QString(">> [%1/%2] 读取: %3")
                                 .arg(i + 1)
                                 .arg(InputFiles.count())
                                 .arg(inputPath));

            auto inputImage = Infrastructure::IO::ReadImage(
                inputPath,
                QStringLiteral("输入影像"),
                context.Log);
            if (!inputImage) {
                ++warningCount;
                continue;
            }

            if (Mode == PixelThresholdMode::BlueBandOnly &&
                inputImage->GetBandCounts() < 3) {
                context.Log.Post("错误: 当前影像波段数不足 3，无法使用蓝色波段阈值检测，已跳过。");
                ++warningCount;
                continue;
            }

            const auto mode = Mode == PixelThresholdMode::BlueBandOnly
                                  ? RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::BlueBandOnly
                                  : RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::Gray;
            const QString modeText =
                Mode == PixelThresholdMode::BlueBandOnly ? QStringLiteral("蓝色波段") : QStringLiteral("灰度");

            RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThreshold algorithm(*inputImage);
            algorithm.SetThreshold(static_cast<unsigned char>(Threshold));
            algorithm.SetThresholdMode(mode);

            context.Log.Post(QString(">> [%1/%2] 正在执行云检测，模式=%3，阈值=%4 ...")
                                 .arg(i + 1)
                                 .arg(InputFiles.count())
                                 .arg(modeText)
                                 .arg(Threshold));
            algorithm.Execute();

            const QString finalSavePath = outputPlan->SingleExactOutput
                                              ? SavePath.trimmed()
                                              : Infrastructure::Execution::BuildOutputPath(
                                                    outputPlan->OutputDir,
                                                    Infrastructure::Execution::BuildBasenameOutputFileName(
                                                        outputPlan->OutputPrefix,
                                                        inputPath));
            if (!Infrastructure::IO::SaveImage(
                    algorithm.AlgorithmResult,
                    finalSavePath,
                    context.Log,
                    QStringLiteral(">> 保存成功: %1"),
                    QStringLiteral("错误: 云掩膜保存失败: %1"))) {
                ++warningCount;
                continue;
            }

            savedPaths.append(finalSavePath);
            ++successCount;
        }

        return BuildBatchSummary(QStringLiteral("CloudDetection::PixelThreshold"),
                                 successCount,
                                 InputFiles.count(),
                                 warningCount,
                                 savedPaths);
    });
}

} // namespace Application::Execution
