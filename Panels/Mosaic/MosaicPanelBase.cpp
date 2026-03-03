#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

MosaicPanelBase::MosaicPanelBase(QWidget *parent) : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void MosaicPanelBase::_SetupUi() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    _ImageSelector = new FileListWidget("输入影像列表 (Images)",
                                        "Images (*.tif *.tiff *.png *.jpg)",
                                        this);
    layout->addWidget(_ImageSelector);

    // 只有基类单独使用时才需要 addStretch，子类可能会添加更多控件
    // 这里为了布局灵活，我们在子类构造完毕后再统一处理 stretch，或者依靠 layout 的特性
}

bool MosaicPanelBase::_SaveResult(const RSPIP::GeoImage &result, const QString &userPath, const QString &prefix) {
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
        PostLog(">> 镶嵌完成并保存！");
    } else {
        PostLog("错误: 结果保存失败。");
    }
    return saved;
}

bool MosaicPanelBase::ValidateInput() {
    if (_ImageSelector->Files().count() < 2) {
        QMessageBox::warning(this, "输入不足", "镶嵌至少需要两张影像。");
        return false;
    }
    return true;
}

} // namespace Panels::Mosaic


