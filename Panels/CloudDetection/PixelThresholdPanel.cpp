#include "PixelThresholdPanel.h"

#include "Algorithm/CloudDetection/PixelThreshold.h"

#include <QLabel>
#include <QVBoxLayout>

namespace Panels::CloudDetection {

PixelThresholdPanel::PixelThresholdPanel(QWidget *parent) : AlgorithmPanelBase(parent) {
    _SetupUi();
}

QString PixelThresholdPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【PixelThreshold】\n\n"
        "功能: 对输入影像列表批量执行云检测，输出二值云掩膜。\n"
        "方法: 使用像素阈值法，可基于灰度或蓝色波段进行阈值检测。\n"
        "参数: 检测通道(灰度/蓝色波段)、阈值范围 0-255，默认 240。");
}

void PixelThresholdPanel::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";
    _InputSelector = new FileListWidget("输入影像列表 (Inputs):", imgFilter, this);
    layout->addWidget(_InputSelector);

    layout->addWidget(new QLabel("阈值检测通道:", this));
    _ModeCombo = new QComboBox(this);
    _ModeCombo->addItem("灰度", static_cast<int>(RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::Gray));
    _ModeCombo->addItem("蓝色波段", static_cast<int>(RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::BlueBandOnly));
    layout->addWidget(_ModeCombo);

    _ThresholdSpin = new QSpinBox(this);
    _ThresholdSpin->setRange(0, 255);
    _ThresholdSpin->setValue(240);
    _ThresholdSpin->setSingleStep(1);
    _ThresholdSpin->setPrefix("云阈值: ");
    layout->addWidget(_ThresholdSpin);

    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> PixelThresholdPanel::ValidateInput() {
    if (_InputSelector->Files().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请至少选择一张输入影像"};
    }
    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
PixelThresholdPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::PixelThresholdRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->InputFiles = _InputSelector ? _InputSelector->Files() : QStringList();
    request->Mode =
        (_ModeCombo && _ModeCombo->currentData().toInt() ==
                           static_cast<int>(RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::BlueBandOnly))
            ? Application::Execution::PixelThresholdMode::BlueBandOnly
            : Application::Execution::PixelThresholdMode::Gray;
    request->Threshold = _ThresholdSpin ? _ThresholdSpin->value() : 240;
    return request;
}

} // namespace Panels::CloudDetection
