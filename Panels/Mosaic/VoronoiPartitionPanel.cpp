#include "VoronoiPartitionPanel.h"

#include "Application/Execution/MosaicRequest.h"

#include <QGroupBox>
#include <QVBoxLayout>

namespace Panels::Mosaic {

VoronoiPartitionPanel::VoronoiPartitionPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

QString VoronoiPartitionPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【VoronoiPartitionMosaic】\n\n"
        "功能: 依据各输入影像有效区域在拼接结果中的种子中心，执行 Voronoi 分区镶嵌。\n"
        "输入: 需要至少两张影像；云掩膜可选，不提供时仅排除 NoData 像素。\n"
        "说明: 如果提供掩膜，则掩膜数量必须与输入影像数量一一对应。");
}

void VoronoiPartitionPanel::_SetupUi() {
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        setLayout(layout);
    }

    auto *maskGroup = new QGroupBox("输入云掩膜 (Cloud Masks，可选)", this);
    auto *maskLayout = new QVBoxLayout(maskGroup);
    maskLayout->setContentsMargins(5, 10, 5, 5);

    _MaskSelector = new FileListWidget("掩膜列表 (可留空；若填写需与影像一一对应)",
                                       "Images (*.tif *.tiff *.png *.jpg)",
                                       this);
    maskLayout->addWidget(_MaskSelector);
    layout->addWidget(maskGroup);
    layout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> VoronoiPartitionPanel::ValidateInput() {
    if (const auto issue = MosaicPanelBase::ValidateInput()) {
        return issue;
    }

    if (_MaskSelector && !_MaskSelector->Files().isEmpty() &&
        _MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        return Infrastructure::Execution::ValidationIssue{
            "数量不匹配",
            "VoronoiPartitionMosaic 在填写掩膜时要求掩膜文件数量必须与影像文件数量完全一致。"};
    }

    return std::nullopt;
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
VoronoiPartitionPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MosaicVoronoiPartitionRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    request->MaskFiles = _MaskSelector ? _MaskSelector->Files() : QStringList();
    return request;
}

} // namespace Panels::Mosaic
