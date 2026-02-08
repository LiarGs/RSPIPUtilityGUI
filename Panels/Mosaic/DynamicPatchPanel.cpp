#include "DynamicPatchPanel.h"

namespace Panels::Mosaic {

DynamicPatchPanel::DynamicPatchPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

void DynamicPatchPanel::_SetupUi() {
    // 获取 BaseMosaicPanel 的布局
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());

    // 添加掩膜选择部分
    auto *maskGroup = new QGroupBox("输入云掩膜 (Cloud Masks)", this);
    auto *maskLayout = new QVBoxLayout(maskGroup);
    maskLayout->setContentsMargins(5, 10, 5, 5);

    _MaskSelector = new FileListWidget("掩膜列表 (需与影像一一对应)",
                                       "Images (*.tif *.tiff *.png *.jpg)",
                                       this);
    maskLayout->addWidget(_MaskSelector);

    // 将掩膜组添加到主布局
    layout->addWidget(maskGroup);
    layout->addStretch();
}

bool DynamicPatchPanel::ValidateInput() const {
    if (!MosaicPanelBase::ValidateInput())
        return false;

    if (_MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        QMessageBox::warning(const_cast<DynamicPatchPanel *>(this), "数量不匹配",
                             "DynamicPatch 算法要求掩膜文件数量必须与影像文件数量完全一致。");
        return false;
    }
    return true;
}

bool DynamicPatchPanel::Run(const QString &globalSavePath) {
    QStringList imageFiles = _ImageSelector->Files();
    QStringList maskFiles = _MaskSelector->Files();

    emit LogMessage(">> [DynamicPatch] 开始执行...");
    QApplication::processEvents();

    try {
        // 1. 读取影像
        std::vector<RSPIP::GeoImage> images;
        images.reserve(imageFiles.count());
        emit LogMessage(">> 正在加载影像数据...");
        for (const QString &path : imageFiles) {
            auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
            if (imgPtr)
                images.push_back(std::move(*imgPtr));
            else {
                emit LogMessage("无法读取: " + path);
                return false;
            }
        }

        // 2. 读取掩膜
        std::vector<RSPIP::CloudMask> masks;
        masks.reserve(maskFiles.count());
        emit LogMessage(">> 正在加载掩膜数据...");
        for (const QString &path : maskFiles) {
            auto maskPtr = RSPIP::IO::CloudMaskImageRead(path.toStdString());
            if (maskPtr)
                masks.push_back(std::move(*maskPtr));
            else {
                emit LogMessage("无法读取掩膜: " + path);
                return false;
            }
        }

        // 3. 执行
        RSPIP::Algorithm::MosaicAlgorithm::DynamicPatch algorithm(images, masks);
        emit LogMessage(">> 正在执行动态补丁镶嵌 (耗时操作)...");
        QApplication::processEvents();
        algorithm.Execute();

        return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "Mosaic_Dynamic");

    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }
}

} // namespace Panels::Mosaic
