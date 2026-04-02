#include "Application/Execution/PreprocessRequest.h"

#include "Application/Execution/AlgorithmRequestCommon.h"

#include "Algorithm/Preprocess/GeoCoordinateAlign.h"
#include "Basic/Image.h"
#include "Infrastructure/Execution/OutputPlanner.h"
#include "Infrastructure/IO/ImageIOService.h"

#include <QFileInfo>

#include <memory>

namespace Application::Execution {

using detail::ExecuteSafely;

Infrastructure::Execution::ExecutionResult GeoCoordinateAlignRequest::Execute(const ExecutionContext &context) const {
    return ExecuteSafely(QStringLiteral("GeoCoordinateAlign 执行失败"), context, [this, &context]() {
        context.Log.Post(QString(">> [GeoCoordinateAlign] 待处理影像数量: %1").arg(ImageFiles.count()));

        const auto outputDir = Infrastructure::Execution::ResolveStrictOutputDirectory(
            SavePath,
            context.Log);
        if (!outputDir) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(">> 正在加载输入影像...");
        const auto images = Infrastructure::IO::ReadImages(
            ImageFiles,
            QStringLiteral("输入影像"),
            context.Log);
        if (!images) {
            return Infrastructure::Execution::ExecutionResult{};
        }

        std::optional<std::vector<RSPIP::Image>> masks = std::nullopt;
        if (!MaskFiles.isEmpty()) {
            context.Log.Post(">> 正在加载掩膜影像...");
            masks = Infrastructure::IO::ReadImages(
                MaskFiles,
                QStringLiteral("掩膜影像"),
                context.Log);
            if (!masks) {
                return Infrastructure::Execution::ExecutionResult{};
            }
        }

        context.Log.Post(">> 正在执行地理坐标对齐...");
        std::unique_ptr<RSPIP::Algorithm::PreprocessAlgorithm::GeoCoordinateAlign> algorithm;
        if (masks) {
            algorithm = std::make_unique<RSPIP::Algorithm::PreprocessAlgorithm::GeoCoordinateAlign>(*images, *masks);
        } else {
            algorithm = std::make_unique<RSPIP::Algorithm::PreprocessAlgorithm::GeoCoordinateAlign>(*images);
        }
        algorithm->Execute();

        if (algorithm->AlignedImages.empty()) {
            context.Log.Post("错误: GeoCoordinateAlign 未生成任何输出，请检查输入影像是否包含有效 GeoInfo。");
            return Infrastructure::Execution::ExecutionResult{};
        }

        context.Log.Post(QString(">> UnifiedGrid: Rows=%1, Columns=%2, Projection=%3")
                             .arg(algorithm->UnifiedGrid.Rows)
                             .arg(algorithm->UnifiedGrid.Columns)
                             .arg(QString::fromStdString(algorithm->UnifiedGrid.Projection)));

        int warningCount = 0;
        QStringList savedPaths;

        for (int i = 0; i < static_cast<int>(algorithm->AlignedImages.size()); ++i) {
            const QString finalSavePath = Infrastructure::Execution::BuildOutputPath(
                *outputDir,
                QString("GeoAlign_img_%1_%2.tif")
                    .arg(i + 1)
                    .arg(QFileInfo(ImageFiles.at(i)).completeBaseName()));
            if (!Infrastructure::IO::SaveImage(
                    algorithm->AlignedImages.at(static_cast<size_t>(i)),
                    finalSavePath,
                    context.Log)) {
                ++warningCount;
                continue;
            }
            savedPaths.append(finalSavePath);
        }

        for (int i = 0; i < static_cast<int>(algorithm->AlignedMaskImages.size()); ++i) {
            const QString finalSavePath = Infrastructure::Execution::BuildOutputPath(
                *outputDir,
                QString("GeoAlign_mask_%1_%2.tif")
                    .arg(i + 1)
                    .arg(QFileInfo(MaskFiles.at(i)).completeBaseName()));
            if (!Infrastructure::IO::SaveImage(
                    algorithm->AlignedMaskImages.at(static_cast<size_t>(i)),
                    finalSavePath,
                    context.Log)) {
                ++warningCount;
                continue;
            }
            savedPaths.append(finalSavePath);
        }

        Infrastructure::Execution::ExecutionResult result;
        result.Success = warningCount == 0;
        result.WarningCount = warningCount;
        result.SavedPaths = savedPaths;
        result.Message = result.Success
                             ? QStringLiteral("GeoCoordinateAlign 执行完成。")
                             : QStringLiteral("GeoCoordinateAlign 已执行完成，但部分结果保存失败。");
        return result;
    });
}

} // namespace Application::Execution
