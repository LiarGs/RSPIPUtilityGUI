#include "RMSEEvaluatorPanel.h"

#include "Algorithm/Evaluation/RMSEEvaluator.h"
#include "IO/ImageReader.h"

#include <exception>
#include <QDateTime>
#include <QTextStream>

namespace Panels::Evaluation {

RMSEEvaluatorPanel::RMSEEvaluatorPanel(QWidget *parent)
    : EvaluatorPanelBase(parent) {
    _SetBoundaryOnlySupported(true);
}

QString RMSEEvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【RMSEEvaluator】\n\n"
        "基于均方根误差（RMSE）评价待评估影像与参考影像之间的整体误差。\n"
        "输入要求：两幅影像尺寸一致，可选传入掩膜限制统计区域。\n"
        "当提供掩膜后，可进一步选择仅统计掩膜边界像素。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::function<bool()> RMSEEvaluatorPanel::BuildTask(const QString &globalSavePath) {
    const QString imagePath = _ImageSelect->CurrentPath();
    const QString referencePath = _ReferenceSelect->CurrentPath();
    const QString maskPath = _MaskSelect ? _MaskSelect->CurrentPath() : QString();
    const bool boundaryOnly = _IsBoundaryOnlyRequested();

    return [this, imagePath, referencePath, maskPath, boundaryOnly, globalSavePath]() {
        PostLog(">> [RMSEEvaluator] 正在读取影像数据...");

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
                PostLog(">> [RMSEEvaluator] 正在读取掩膜数据...");

                maskImage = RSPIP::IO::NormalImageRead(maskPath.toStdString());

                if (!maskImage) {
                    PostLog("错误: 无法读取掩膜数据。");
                    return false;
                }
                if (!_ValidateMaskCompatible(*imageData, *maskImage)) {
                    return false;
                }
            }

            PostLog(">> [RMSEEvaluator] 正在计算 RMSE...");
            double result = 0.0;
            if (maskImage) {
                RSPIP::Algorithm::RMSEEvaluator algorithm(*imageData, *referenceImage, *maskImage);
                algorithm.SetBoundaryOnly(boundaryOnly);
                algorithm.Execute();
                result = algorithm.EvaluateResult;
            } else {
                RSPIP::Algorithm::RMSEEvaluator algorithm(*imageData, *referenceImage);
                algorithm.Execute();
                result = algorithm.EvaluateResult;
            }

            PostLog(QString(">> RMSE = %1").arg(result, 0, 'f', 6));

            QString content;
            QTextStream stream(&content);
            stream << "Metric: RMSE\n";
            stream << "Value: " << QString::number(result, 'f', 6) << "\n";
            stream << "Image: " << imagePath << "\n";
            stream << "Reference: " << referencePath << "\n";
            stream << "Mask: " << (maskPath.isEmpty() ? "<none>" : maskPath) << "\n";
            stream << "BoundaryOnly: " << (boundaryOnly ? "true" : "false") << "\n";
            stream << "GeneratedAt: "
                   << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                   << "\n";

            return _SaveTextResult(content, globalSavePath, "rmse_eval");
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Evaluation
