#include "AdaptivePatchPanel.h"

namespace Panels::Mosaic {

AdaptivePatchPanel::AdaptivePatchPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

void AdaptivePatchPanel::_SetupUi() {
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        setLayout(layout);
    }

    auto *maskGroup = new QGroupBox("输入云掩膜 (Cloud Masks)", this);
    auto *maskLayout = new QVBoxLayout(maskGroup);
    maskLayout->setContentsMargins(5, 10, 5, 5);

    _MaskSelector = new FileListWidget("掩膜列表 (需与影像一一对应)",
                                       "Images (*.tif *.tiff *.png *.jpg)",
                                       this);
    maskLayout->addWidget(_MaskSelector);
    layout->addWidget(maskGroup);

    auto *parameterGroup = new QGroupBox("镶嵌参数", this);
    auto *parameterLayout = new QVBoxLayout(parameterGroup);
    parameterLayout->setContentsMargins(5, 10, 5, 5);

    _StripWidthSpin = new QSpinBox(this);
    _StripWidthSpin->setRange(1, 1000000);
    _StripWidthSpin->setValue(32);
    _StripWidthSpin->setPrefix("条带宽度: ");
    parameterLayout->addWidget(_StripWidthSpin);

    layout->addWidget(parameterGroup);
    layout->addStretch();
}

bool AdaptivePatchPanel::ValidateInput() {
    if (!MosaicPanelBase::ValidateInput())
        return false;

    if (_MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        QMessageBox::warning(this, "数量不匹配",
                             "AdaptivePatch 算法要求掩膜文件数量必须与影像文件数量完全一致。");
        return false;
    }
    return true;
}

std::function<bool()> AdaptivePatchPanel::BuildTask(const QString &globalSavePath) {
    const QStringList imageFiles = _ImageSelector->Files();
    const QStringList maskFiles = _MaskSelector->Files();
    const int stripWidth = _StripWidthSpin ? _StripWidthSpin->value() : 32;

    return [this, imageFiles, maskFiles, stripWidth, globalSavePath]() {
        PostLog(">> [AdaptivePatch] 开始执行...");

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

            RSPIP::Algorithm::MosaicAlgorithm::AdaptivePatch algorithm(images, masks);
            algorithm.SetStripWidth(stripWidth);
            PostLog(QString(">> 镶嵌参数: 条带宽度=%1").arg(stripWidth));
            PostLog(">> 正在执行自适应补丁镶嵌 (耗时操作)...");
            algorithm.Execute();

            return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "Mosaic_AdaptivePatch");

        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::Mosaic
