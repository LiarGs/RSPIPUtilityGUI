#include "ReconstructPanelBase.h"
#include "Common/FileSelectWidget.h"
#include "IO/ImageSaveVisitor.h"

namespace Panels::Reconstruct {

ReconstructPanelBase::ReconstructPanelBase(QWidget *parent) : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void ReconstructPanelBase::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";

    _TargetSelect = new FileSelectWidget("待修复影像 (Target):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_TargetSelect);

    _ReferSelect = new FileSelectWidget("参考影像 (Reference):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_ReferSelect);

    _MaskSelect = new FileSelectWidget("云掩膜 (Mask):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_MaskSelect);

    layout->addStretch();
}

bool ReconstructPanelBase::ValidateInput() const {
    if (_TargetSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(const_cast<ReconstructPanelBase *>(this), "输入错误", "请选择目标影像 (Target)");
        return false;
    }
    if (_ReferSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(const_cast<ReconstructPanelBase *>(this), "输入错误", "请选择参考影像 (Reference)");
        return false;
    }
    if (_MaskSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(const_cast<ReconstructPanelBase *>(this), "输入错误", "请选择云掩膜 (Mask)");
        return false;
    }

    return true;
}

bool ReconstructPanelBase::_SaveResult(const RSPIP::Image &result, const QString &userPath, const QString &prefix) {
    QString finalSavePath = userPath;
    if (finalSavePath.isEmpty()) {
        QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        finalSavePath = QString("%1/%2_result_%3.tif")
                            .arg(tempDir)
                            .arg(prefix)
                            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
        PostLog(">> 使用自动生成的路径: " + finalSavePath);
    }

    bool saved = RSPIP::IO::SaveImage(result,
                                      QFileInfo(finalSavePath).absolutePath().toStdString(),
                                      QFileInfo(finalSavePath).fileName().toStdString());

    if (saved) {
        PostLog(">> 重构完成并保存！");
    } else {
        PostLog("错误: 结果保存失败。");
    }
    return saved;
}

} // namespace Panels::Reconstruct
