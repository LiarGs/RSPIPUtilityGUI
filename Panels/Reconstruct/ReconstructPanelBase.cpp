#include "ReconstructPanelBase.h"

namespace Panels::Reconstruct {

ReconstructPanelBase::ReconstructPanelBase(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void ReconstructPanelBase::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    const QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";

    _TargetSelect = new FileSelectWidget("待修复影像 (Target):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_TargetSelect);

    _ReferSelect = new FileSelectWidget("参考影像 (Reference):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_ReferSelect);

    _MaskSelect = new FileSelectWidget("云掩膜 (Mask):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_MaskSelect);

    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> ReconstructPanelBase::ValidateInput() {
    if (_TargetSelect->CurrentPath().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请选择目标影像 (Target)"};
    }
    if (_ReferSelect->CurrentPath().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请选择参考影像 (Reference)"};
    }
    if (_MaskSelect->CurrentPath().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请选择云掩膜 (Mask)"};
    }

    return std::nullopt;
}

void ReconstructPanelBase::_PopulateSingleImageRequest(Application::Execution::SingleImageRequest &request,
                                                       const QString &savePath) const {
    request.SavePath = savePath.trimmed();
    request.TargetPath = _TargetSelect ? _TargetSelect->CurrentPath() : QString();
    request.ReferencePath = _ReferSelect ? _ReferSelect->CurrentPath() : QString();
    request.MaskPath = _MaskSelect ? _MaskSelect->CurrentPath() : QString();
}

} // namespace Panels::Reconstruct
