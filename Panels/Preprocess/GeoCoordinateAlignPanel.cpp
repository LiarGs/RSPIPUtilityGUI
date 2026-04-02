#include "GeoCoordinateAlignPanel.h"

#include <QVBoxLayout>

namespace Panels::Preprocess {

GeoCoordinateAlignPanel::GeoCoordinateAlignPanel(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

QString GeoCoordinateAlignPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【GeoCoordinateAlign】\n\n"
        "对多幅带 GeoInfo 的影像进行统一网格对齐，并可同步对齐对应的 mask image。\n"
        "输入要求：至少一幅输入影像，且每幅影像都必须包含有效 GeoInfo；"
        "若提供 mask 列表，则数量必须与影像数量完全一致。\n"
        "输出形式：将对齐后的影像和掩膜批量保存到输出目录。");
}

void GeoCoordinateAlignPanel::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    const QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.jpeg *.bmp)";

    _ImageSelector = new FileListWidget("输入影像列表 (Images):", imgFilter, this);
    layout->addWidget(_ImageSelector);

    _MaskSelector = new FileListWidget("输入掩膜列表 (Masks, 可选，需与影像一一对应):", imgFilter, this);
    layout->addWidget(_MaskSelector);

    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> GeoCoordinateAlignPanel::ValidateInput() {
    if (_ImageSelector->Files().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请至少选择一张输入影像"};
    }

    if (!_MaskSelector->Files().isEmpty() &&
        _MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        return Infrastructure::Execution::ValidationIssue{
            "数量不匹配",
            "如果提供掩膜列表，其数量必须与输入影像数量完全一致。"};
    }

    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
GeoCoordinateAlignPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::GeoCoordinateAlignRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    request->MaskFiles = _MaskSelector ? _MaskSelector->Files() : QStringList();
    return request;
}

} // namespace Panels::Preprocess
