#include "AdaptivePatchPanel.h"

#include <QGroupBox>
#include <QVBoxLayout>

namespace Panels::Mosaic {

AdaptivePatchPanel::AdaptivePatchPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

void AdaptivePatchPanel::_SetupUi() {
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        setLayout(layout);
    }

    auto *maskGroup = new QGroupBox("输入云掩膜 (Cloud Masks)", this);
    auto *maskLayout = new QVBoxLayout(maskGroup);
    maskLayout->setContentsMargins(5, 10, 5, 5);

    _MaskSelector = new FileListWidget("掩膜列表 (需与影像一一对应)",
                                       "Images (*.tif *.tiff *.png *.jpg)",
                                       this);
    maskLayout->addWidget(_MaskSelector);
    layout->addWidget(maskGroup);

    auto *parameterGroup = new QGroupBox("镶嵌参数", this);
    auto *parameterLayout = new QVBoxLayout(parameterGroup);
    parameterLayout->setContentsMargins(5, 10, 5, 5);

    _StripWidthSpin = new QSpinBox(this);
    _StripWidthSpin->setRange(1, 1000000);
    _StripWidthSpin->setValue(32);
    _StripWidthSpin->setPrefix("条带宽度: ");
    parameterLayout->addWidget(_StripWidthSpin);

    layout->addWidget(parameterGroup);
    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> AdaptivePatchPanel::ValidateInput() {
    if (const auto issue = MosaicPanelBase::ValidateInput()) {
        return issue;
    }

    if (_MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        return Infrastructure::Execution::ValidationIssue{
            "数量不匹配",
            "AdaptivePatch 算法要求掩膜文件数量必须与影像文件数量完全一致。"};
    }
    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
AdaptivePatchPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MosaicAdaptivePatchRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    request->MaskFiles = _MaskSelector ? _MaskSelector->Files() : QStringList();
    request->StripWidth = _StripWidthSpin ? _StripWidthSpin->value() : 32;
    return request;
}

} // namespace Panels::Mosaic
