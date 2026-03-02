#include "IsophotePanel.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
// --- RSPIP Headers ---
#include "Basic/CloudMask.h"
#include "IO/ImageReader.h"

namespace Panels::Reconstruct {

IsophotePanel::IsophotePanel(QWidget *parent) : ReconstructPanelBase(parent) {
    _SetupUi();
}

void IsophotePanel::_SetupUi() {
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        setLayout(layout);
    }

    auto *paramGroup = new QGroupBox("算法参数设置", this);
    auto *paramLayout = new QFormLayout(paramGroup);
    paramLayout->setContentsMargins(10, 15, 10, 10);
    paramLayout->setSpacing(8);

    _MaxIterationsSpinBox = new QSpinBox(this);
    _MaxIterationsSpinBox->setRange(1, 10000);
    _MaxIterationsSpinBox->setValue(1000);
    _MaxIterationsSpinBox->setSingleStep(10);
    _MaxIterationsSpinBox->setSuffix(" 次");
    _MaxIterationsSpinBox->setToolTip("算法迭代的最大次数，值越大结果可能越精确但耗时越长");

    _EpsilonSpinBox = new QDoubleSpinBox(this);
    _EpsilonSpinBox->setRange(0.0001, 100.0);
    _EpsilonSpinBox->setValue(1.0);
    _EpsilonSpinBox->setSingleStep(0.1);
    _EpsilonSpinBox->setDecimals(4);
    _EpsilonSpinBox->setToolTip("收敛阈值，当变化小于该值时停止迭代");

    paramLayout->addRow("最大迭代次数:", _MaxIterationsSpinBox);
    paramLayout->addRow("收敛阈值 (ε):", _EpsilonSpinBox);

    layout->addWidget(paramGroup);
    layout->addStretch();
}

std::function<bool()> IsophotePanel::BuildTask(const QString &globalSavePath) {
    const QString targetPath = _TargetSelect->CurrentPath();
    const QString referPath = _ReferSelect->CurrentPath();
    const QString maskPath = _MaskSelect->CurrentPath();
    const int currentMaxIterations = _MaxIterationsSpinBox->value();
    const double currentEpsilon = _EpsilonSpinBox->value();

    return [this,
            targetPath,
            referPath,
            maskPath,
            currentMaxIterations,
            currentEpsilon,
            globalSavePath]() {
        PostLog(">> 正在加载影像数据...");

        auto targetImage = RSPIP::IO::GeoImageRead(targetPath.toStdString());
        auto referImage = RSPIP::IO::GeoImageRead(referPath.toStdString());
        auto maskImage = RSPIP::IO::CloudMaskImageRead(maskPath.toStdString());

        if (!(targetImage && referImage && maskImage)) {
            PostLog("错误: 无法读取影像或掩膜文件。");
            return false;
        }

        try {
            PostLog(">> [IsophoteConstrain] 正在执行 (可能耗时较长)...");

            RSPIP::Algorithm::ReconstructAlgorithm::IsophoteConstrain algorithm(*targetImage, *referImage, *maskImage);
            algorithm.SetMaxIterations(currentMaxIterations);
            algorithm.SetEpsilon(currentEpsilon);
            algorithm.Execute();

            return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "ReconstructIsophote");
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Reconstruct


