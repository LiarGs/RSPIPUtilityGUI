#include "PixelWiseStrategyPanel.h"

#include "Application/Execution/MosaicRequest.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

namespace Panels::Mosaic {

namespace {

using Application::Execution::MosaicPixelWiseOverlapStrategy;

} // namespace

PixelWiseStrategyPanel::PixelWiseStrategyPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

QString PixelWiseStrategyPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【PixelWiseStrategyMosaic】\n\n"
        "功能: 将多张影像按像素级策略执行统一镶嵌。\n"
        "策略: 可选择直接覆盖、重叠区域高亮为红色、对有效像素求均值，或按有效像素中值滤波。\n"
        "说明: MeanOfValidPixels 与 MedianOfValidPixels 可使用云掩膜；若提供掩膜则要求与输入影像一一对应，不提供时仅排除 NoData。");
}

void PixelWiseStrategyPanel::_SetupUi() {
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        setLayout(layout);
    }

    layout->setSpacing(5);

    layout->addWidget(new QLabel("像素替换策略:", this));
    _StrategyCombo = new QComboBox(this);
    _StrategyCombo->addItem(QStringLiteral("直接覆盖 (LastWriteWins)"),
                            static_cast<int>(MosaicPixelWiseOverlapStrategy::LastWriteWins));
    _StrategyCombo->addItem(QStringLiteral("重叠高亮红色 (HighlightOverlapRed)"),
                            static_cast<int>(MosaicPixelWiseOverlapStrategy::HighlightOverlapRed));
    _StrategyCombo->addItem(QStringLiteral("有效像素均值 (MeanOfValidPixels)"),
                            static_cast<int>(MosaicPixelWiseOverlapStrategy::MeanOfValidPixels));
    _StrategyCombo->addItem(QStringLiteral("有效像素中值滤波 (MedianOfValidPixels)"),
                            static_cast<int>(MosaicPixelWiseOverlapStrategy::MedianOfValidPixels));
    layout->addWidget(_StrategyCombo);

    _MaskGroup = new QGroupBox("输入云掩膜 (Cloud Masks)", this);
    auto *maskLayout = new QVBoxLayout(_MaskGroup);
    maskLayout->setContentsMargins(5, 10, 5, 5);

    _MaskSelector = new FileListWidget("掩膜列表 (Mean/Median 策略可选；若填写需与影像一一对应)",
                                       "Images (*.tif *.tiff *.png *.jpg)",
                                       this);
    maskLayout->addWidget(_MaskSelector);
    layout->addWidget(_MaskGroup);
    layout->addStretch();

    connect(_StrategyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { _UpdateMaskVisibility(); });

    _UpdateMaskVisibility();
}

std::optional<Infrastructure::Execution::ValidationIssue> PixelWiseStrategyPanel::ValidateInput() {
    if (const auto issue = MosaicPanelBase::ValidateInput()) {
        return issue;
    }

    if (_UsesAggregateMaskInputs() &&
        _MaskSelector &&
        !_MaskSelector->Files().isEmpty() &&
        _MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        return Infrastructure::Execution::ValidationIssue{
            "数量不匹配",
            "MeanOfValidPixels / MedianOfValidPixels 策略在提供掩膜时要求掩膜文件数量必须与输入影像数量完全一致。"};
    }

    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
PixelWiseStrategyPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MosaicPixelWiseStrategyRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    request->Strategy = _StrategyCombo
                            ? static_cast<Application::Execution::MosaicPixelWiseOverlapStrategy>(_StrategyCombo->currentData().toInt())
                            : Application::Execution::MosaicPixelWiseOverlapStrategy::LastWriteWins;
    request->MaskFiles = (_UsesAggregateMaskInputs() && _MaskSelector) ? _MaskSelector->Files() : QStringList();
    return request;
}

void PixelWiseStrategyPanel::_UpdateMaskVisibility() {
    if (_MaskGroup) {
        _MaskGroup->setVisible(_UsesAggregateMaskInputs());
    }
}

bool PixelWiseStrategyPanel::_UsesAggregateMaskInputs() const {
    return _StrategyCombo &&
           (static_cast<Application::Execution::MosaicPixelWiseOverlapStrategy>(_StrategyCombo->currentData().toInt()) ==
                Application::Execution::MosaicPixelWiseOverlapStrategy::MeanOfValidPixels ||
            static_cast<Application::Execution::MosaicPixelWiseOverlapStrategy>(_StrategyCombo->currentData().toInt()) ==
                Application::Execution::MosaicPixelWiseOverlapStrategy::MedianOfValidPixels);
}

} // namespace Panels::Mosaic
