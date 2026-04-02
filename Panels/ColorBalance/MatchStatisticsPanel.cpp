#include "MatchStatisticsPanel.h"
#include "Common/FileListWidget.h"
#include "Common/FileSelectWidget.h"

#include <QVBoxLayout>
#include <utility>

namespace Panels::ColorBalance {

MatchStatisticsPanel::MatchStatisticsPanel(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void MatchStatisticsPanel::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";

    _ReferSelect = new FileSelectWidget("基准影像 (Reference):",
                                        imgFilter,
                                        FileSelectWidget::Mode::FileOpen,
                                        this);
    layout->addWidget(_ReferSelect);

    _InputSelector = new FileListWidget("待匀色影像列表 (Targets):", imgFilter, this);
    layout->addWidget(_InputSelector);

    _MaskSelector = new FileListWidget("掩膜列表 (Mask，可选，需与待匀色影像一一对应):",
                                       imgFilter,
                                       this);
    layout->addWidget(_MaskSelector);

    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> MatchStatisticsPanel::ValidateInput() {
    if (_ReferSelect->CurrentPath().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请选择基准影像"};
    }
    if (_InputSelector->Files().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请至少选择一张待匀色影像"};
    }
    if (!_MaskSelector->Files().isEmpty() &&
        _MaskSelector->Files().count() != _InputSelector->Files().count()) {
        return Infrastructure::Execution::ValidationIssue{
            "数量不匹配",
            "如果提供掩膜列表，其数量必须与待匀色影像数量完全一致。"};
    }
    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
MatchStatisticsPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MatchStatisticsRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ReferencePath = _ReferSelect ? _ReferSelect->CurrentPath() : QString();
    request->InputFiles = _InputSelector ? _InputSelector->Files() : QStringList();
    request->MaskFiles = _MaskSelector ? _MaskSelector->Files() : QStringList();
    return request;
}

} // namespace Panels::ColorBalance
