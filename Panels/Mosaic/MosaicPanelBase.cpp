#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

MosaicPanelBase::MosaicPanelBase(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void MosaicPanelBase::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    _ImageSelector = new FileListWidget("输入影像列表 (Images)",
                                        "Images (*.tif *.tiff *.png *.jpg)",
                                        this);
    layout->addWidget(_ImageSelector);
}

std::optional<Infrastructure::Execution::ValidationIssue> MosaicPanelBase::ValidateInput() {
    if (_ImageSelector->Files().count() < 2) {
        return Infrastructure::Execution::ValidationIssue{"输入不足", "镶嵌至少需要两张影像。"};
    }
    return std::nullopt;
}

} // namespace Panels::Mosaic
