#include "PSNREvaluatorPanel.h"

#include "Algorithm/Evaluation/PSNREvaluator.h"
#include "IO/ImageReader.h"

#include <exception>
#include <QDateTime>
#include <QTextStream>

namespace Panels::Evaluation {

QString PSNREvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【PSNREvaluator】\n\n"
        "基于峰值信噪比（PSNR）评价待评估影像与参考影像之间的整体误差。\n"
        "输入要求：两幅影像尺寸一致、波段数一致，可选传入掩膜限制统计区域。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::function<bool()> PSNREvaluatorPanel::BuildTask(const QString &globalSavePath) {
    const QString imagePath = _ImageSelect->CurrentPath();
    const QString referencePath = _ReferenceSelect->CurrentPath();
    const QString maskPath = _MaskSelect ? _MaskSelect->CurrentPath() : QString();

    return [this, imagePath, referencePath, maskPath, globalSavePath]() {
        PostLog(">> [PSNREvaluator] 正在读取影像数据...");

        try {
            std::unique_ptr<RSPIP::Image> imageData;
            imageData = RSPIP::IO::NormalImageRead(imagePath.toStdString());

            std::unique_ptr<RSPIP::Image> referenceImage;
            referenceImage = RSPIP::IO::NormalImageRead(referencePath.toStdString());

            if (!imageData || !referenceImage) {
                PostLog("错误: 无法读取待评估影像或参考影像。");
                return false;
            }

            if (!_ValidatePairwiseCompatible(*imageData, *referenceImage)) {
                return false;
            }

            std::unique_ptr<RSPIP::Image> maskImage;
            if (!maskPath.isEmpty()) {
                PostLog(">> [PSNREvaluator] 正在读取掩膜数据...");

                maskImage = RSPIP::IO::NormalImageRead(maskPath.toStdString());

                if (!maskImage) {
                    PostLog("错误: 无法读取掩膜数据。");
                    return false;
                }
                if (!_ValidateMaskCompatible(*imageData, *maskImage)) {
                    return false;
                }
            }

            PostLog(">> [PSNREvaluator] 正在计算 PSNR...");
            double result = 0.0;
            if (maskImage) {
                RSPIP::Algorithm::PSNREvaluator algorithm(*imageData, *referenceImage, *maskImage);
                algorithm.Execute();
                result = algorithm.EvaluateResult;
            } else {
                RSPIP::Algorithm::PSNREvaluator algorithm(*imageData, *referenceImage);
                algorithm.Execute();
                result = algorithm.EvaluateResult;
            }

            PostLog(QString(">> PSNR = %1 dB").arg(result, 0, 'f', 6));

            QString content;
            QTextStream stream(&content);
            stream << "Metric: PSNR\n";
            stream << "Value: " << QString::number(result, 'f', 6) << " dB\n";
            stream << "Image: " << imagePath << "\n";
            stream << "Reference: " << referencePath << "\n";
            stream << "Mask: " << (maskPath.isEmpty() ? "<none>" : maskPath) << "\n";
            stream << "GeneratedAt: "
                   << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                   << "\n";

            return _SaveTextResult(content, globalSavePath, "psnr_eval");
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Evaluation
