#include "IsophotePanel.h"

#include <QFormLayout>
#include <QGroupBox>

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

std::unique_ptr<Application::Execution::AlgorithmRequest>
IsophotePanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::ReconstructIsophoteRequest>();
    _PopulateSingleImageRequest(*request, globalSavePath);
    request->MaxIterations = _MaxIterationsSpinBox ? _MaxIterationsSpinBox->value() : 1000;
    request->Epsilon = _EpsilonSpinBox ? _EpsilonSpinBox->value() : 1.0;
    return request;
}

} // namespace Panels::Reconstruct
