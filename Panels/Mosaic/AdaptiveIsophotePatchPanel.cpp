#include "AdaptiveIsophotePatchPanel.h"

namespace Panels::Mosaic {

AdaptiveIsophotePatchPanel::AdaptiveIsophotePatchPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

void AdaptiveIsophotePatchPanel::_SetupUi() {
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());

    auto *maskGroup = new QGroupBox("输入云掩膜 (Cloud Masks)", this);
    auto *maskLayout = new QVBoxLayout(maskGroup);
    maskLayout->setContentsMargins(5, 10, 5, 5);

    _MaskSelector = new FileListWidget("掩膜列表 (需与影像一一对应)",
                                       "Images (*.tif *.tiff *.png *.jpg)",
                                       this);
    maskLayout->addWidget(_MaskSelector);

    layout->addWidget(maskGroup);
    layout->addStretch();
}

bool AdaptiveIsophotePatchPanel::ValidateInput() const {
    if (!MosaicPanelBase::ValidateInput())
        return false;

    if (_MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        QMessageBox::warning(const_cast<AdaptiveIsophotePatchPanel *>(this), "数量不匹配",
                             "AdaptiveIsophotePatch 算法要求掩膜文件数量必须与影像文件数量完全一致。");
        return false;
    }
    return true;
}

std::function<bool()> AdaptiveIsophotePatchPanel::BuildTask(const QString &globalSavePath) {
    const QStringList imageFiles = _ImageSelector->Files();
    const QStringList maskFiles = _MaskSelector->Files();

    return [this, imageFiles, maskFiles, globalSavePath]() {
        PostLog(">> [AdaptiveIsophotePatch] 开始执行...");

        try {
            std::vector<RSPIP::GeoImage> images;
            images.reserve(imageFiles.count());
            PostLog(">> 正在加载影像数据...");
            for (const QString &path : imageFiles) {
                auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
                if (imgPtr) {
                    images.push_back(std::move(*imgPtr));
                } else {
                    PostLog("无法读取: " + path);
                    return false;
                }
            }

            std::vector<RSPIP::CloudMask> masks;
            masks.reserve(maskFiles.count());
            PostLog(">> 正在加载掩膜数据...");
            for (const QString &path : maskFiles) {
                auto maskPtr = RSPIP::IO::CloudMaskImageRead(path.toStdString());
                if (maskPtr) {
                    masks.push_back(std::move(*maskPtr));
                } else {
                    PostLog("无法读取掩膜: " + path);
                    return false;
                }
            }

            RSPIP::Algorithm::MosaicAlgorithm::AdaptiveIsophotePatch algorithm(images, masks);
            PostLog(">> 正在执行等照度自适应补丁镶嵌 (耗时操作)...");
            algorithm.Execute();

            return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "Mosaic_AdaptiveIsophote");

        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Mosaic


