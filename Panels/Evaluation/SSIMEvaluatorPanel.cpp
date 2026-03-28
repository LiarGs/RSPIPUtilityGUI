#include "SSIMEvaluatorPanel.h"

#include "Algorithm/Evaluation/SSIMEvaluator.h"
#include "IO/ImageReader.h"

#include <exception>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QTextStream>

namespace Panels::Evaluation {

SSIMEvaluatorPanel::SSIMEvaluatorPanel(QWidget *parent)
    : EvaluatorPanelBase(parent) {
    auto *paramGroup = new QGroupBox("算法参数", this);
    auto *formLayout = new QFormLayout(paramGroup);

    _K1Spin = new QDoubleSpinBox(paramGroup);
    _K1Spin->setDecimals(4);
    _K1Spin->setRange(0.0, 1.0);
    _K1Spin->setSingleStep(0.01);
    _K1Spin->setValue(0.01);
    formLayout->addRow("K1:", _K1Spin);

    _K2Spin = new QDoubleSpinBox(paramGroup);
    _K2Spin->setDecimals(4);
    _K2Spin->setRange(0.0, 1.0);
    _K2Spin->setSingleStep(0.01);
    _K2Spin->setValue(0.03);
    formLayout->addRow("K2:", _K2Spin);

    _SetBoundaryOnlySupported(true);

    if (_MainLayout) {
        const int insertIndex = _MainLayout->count() > 0 ? _MainLayout->count() - 1 : 0;
        _MainLayout->insertWidget(insertIndex, paramGroup);
    }
}

QString SSIMEvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【SSIMEvaluator】\n\n"
        "基于结构相似性指数（SSIM）衡量待评估影像与参考影像在亮度、对比度和结构上的一致性。\n"
        "输入要求：两幅影像尺寸一致，可选传入掩膜限制统计区域。\n"
        "参数说明：K1 与 K2 用于稳定亮度项和对比度项，默认值分别为 0.01 与 0.03；"
        "当提供掩膜后，可进一步选择仅统计掩膜边界像素。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::function<bool()> SSIMEvaluatorPanel::BuildTask(const QString &globalSavePath) {
    const QString imagePath = _ImageSelect->CurrentPath();
    const QString referencePath = _ReferenceSelect->CurrentPath();
    const QString maskPath = _MaskSelect ? _MaskSelect->CurrentPath() : QString();
    const bool boundaryOnly = _IsBoundaryOnlyRequested();
    const double k1 = _K1Spin ? _K1Spin->value() : 0.01;
    const double k2 = _K2Spin ? _K2Spin->value() : 0.03;

    return [this, imagePath, referencePath, maskPath, boundaryOnly, k1, k2, globalSavePath]() {
        PostLog(">> [SSIMEvaluator] 正在读取影像数据...");

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
                PostLog(">> [SSIMEvaluator] 正在读取掩膜数据...");

                maskImage = RSPIP::IO::NormalImageRead(maskPath.toStdString());

                if (!maskImage) {
                    PostLog("错误: 无法读取掩膜数据。");
                    return false;
                }
                if (!_ValidateMaskCompatible(*imageData, *maskImage)) {
                    return false;
                }
            }

            PostLog(">> [SSIMEvaluator] 正在计算 SSIM...");
            double result = 0.0;
            if (maskImage) {
                RSPIP::Algorithm::SSIMEvaluator algorithm(*imageData, *referenceImage, *maskImage);
                algorithm.SetK1(k1);
                algorithm.SetK2(k2);
                algorithm.SetBoundaryOnly(boundaryOnly);
                algorithm.Execute();
                result = algorithm.EvaluateResult;
            } else {
                RSPIP::Algorithm::SSIMEvaluator algorithm(*imageData, *referenceImage);
                algorithm.SetK1(k1);
                algorithm.SetK2(k2);
                algorithm.Execute();
                result = algorithm.EvaluateResult;
            }

            PostLog(QString(">> SSIM = %1").arg(result, 0, 'f', 6));

            QString content;
            QTextStream stream(&content);
            stream << "Metric: SSIM\n";
            stream << "Value: " << QString::number(result, 'f', 6) << "\n";
            stream << "K1: " << QString::number(k1, 'f', 4) << "\n";
            stream << "K2: " << QString::number(k2, 'f', 4) << "\n";
            stream << "Image: " << imagePath << "\n";
            stream << "Reference: " << referencePath << "\n";
            stream << "Mask: " << (maskPath.isEmpty() ? "<none>" : maskPath) << "\n";
            stream << "BoundaryOnly: " << (boundaryOnly ? "true" : "false") << "\n";
            stream << "GeneratedAt: "
                   << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                   << "\n";

            return _SaveTextResult(content, globalSavePath, "ssim_eval");
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Evaluation
