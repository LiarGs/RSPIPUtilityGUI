#include "AdaptiveIsophotePatchPanel.h"

#include <QGroupBox>
#include <QVBoxLayout>

namespace Panels::Mosaic {

AdaptiveIsophotePatchPanel::AdaptiveIsophotePatchPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

void AdaptiveIsophotePatchPanel::_SetupUi() {
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

    auto *solverGroup = new QGroupBox("求解参数", this);
    auto *solverLayout = new QVBoxLayout(solverGroup);
    solverLayout->setContentsMargins(5, 10, 5, 5);

    _StripWidthSpin = new QSpinBox(this);
    _StripWidthSpin->setRange(1, 1000000);
    _StripWidthSpin->setValue(32);
    _StripWidthSpin->setPrefix("条带宽度: ");
    solverLayout->addWidget(_StripWidthSpin);

    _MaxIterationsSpin = new QSpinBox(this);
    _MaxIterationsSpin->setRange(1, 1000000);
    _MaxIterationsSpin->setValue(10000);
    _MaxIterationsSpin->setPrefix("最大迭代次数: ");
    solverLayout->addWidget(_MaxIterationsSpin);

    _EpsilonSpin = new QDoubleSpinBox(this);
    _EpsilonSpin->setRange(0.0, 1e9);
    _EpsilonSpin->setDecimals(6);
    _EpsilonSpin->setSingleStep(0.1);
    _EpsilonSpin->setValue(1.0);
    _EpsilonSpin->setPrefix("残差: ");
    solverLayout->addWidget(_EpsilonSpin);

    layout->addWidget(solverGroup);
    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> AdaptiveIsophotePatchPanel::ValidateInput() {
    if (const auto issue = MosaicPanelBase::ValidateInput()) {
        return issue;
    }

    if (_MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        return Infrastructure::Execution::ValidationIssue{
            "数量不匹配",
            "AdaptiveIsophotePatch 算法要求掩膜文件数量必须与影像文件数量完全一致。"};
    }
    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
AdaptiveIsophotePatchPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MosaicAdaptiveIsophotePatchRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    request->MaskFiles = _MaskSelector ? _MaskSelector->Files() : QStringList();
    request->StripWidth = _StripWidthSpin ? _StripWidthSpin->value() : 32;
    request->MaxIterations = _MaxIterationsSpin ? _MaxIterationsSpin->value() : 10000;
    request->Epsilon = _EpsilonSpin ? _EpsilonSpin->value() : 1.0;
    return request;
}

} // namespace Panels::Mosaic
