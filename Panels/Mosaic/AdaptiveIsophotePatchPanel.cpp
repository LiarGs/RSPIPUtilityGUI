#include "AdaptiveIsophotePatchPanel.h"

namespace Panels::Mosaic {

AdaptiveIsophotePatchPanel::AdaptiveIsophotePatchPanel(QWidget *parent) : MosaicPanelBase(parent) {
    _SetupUi();
}

void AdaptiveIsophotePatchPanel::_SetupUi() {
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

    auto *solverGroup = new QGroupBox("求解参数", this);
    auto *solverLayout = new QVBoxLayout(solverGroup);
    solverLayout->setContentsMargins(5, 10, 5, 5);

    _StripWidthSpin = new QSpinBox(this);
    _StripWidthSpin->setRange(1, 1000000);
    _StripWidthSpin->setValue(32);
    _StripWidthSpin->setPrefix("条带宽度: ");
    solverLayout->addWidget(_StripWidthSpin);

    _MaxIterationsSpin = new QSpinBox(this);
    _MaxIterationsSpin->setRange(1, 1000000);
    _MaxIterationsSpin->setValue(10000);
    _MaxIterationsSpin->setPrefix("最大迭代次数: ");
    solverLayout->addWidget(_MaxIterationsSpin);

    _EpsilonSpin = new QDoubleSpinBox(this);
    _EpsilonSpin->setRange(0.0, 1e9);
    _EpsilonSpin->setDecimals(6);
    _EpsilonSpin->setSingleStep(0.1);
    _EpsilonSpin->setValue(1.0);
    _EpsilonSpin->setPrefix("残差: ");
    solverLayout->addWidget(_EpsilonSpin);

    layout->addWidget(solverGroup);
    layout->addStretch();
}

bool AdaptiveIsophotePatchPanel::ValidateInput() {
    if (!MosaicPanelBase::ValidateInput())
        return false;

    if (_MaskSelector->Files().count() != _ImageSelector->Files().count()) {
        QMessageBox::warning(this, "数量不匹配",
                             "AdaptiveIsophotePatch 算法要求掩膜文件数量必须与影像文件数量完全一致。");
        return false;
    }
    return true;
}

std::function<bool()> AdaptiveIsophotePatchPanel::BuildTask(const QString &globalSavePath) {
    const QStringList imageFiles = _ImageSelector->Files();
    const QStringList maskFiles = _MaskSelector->Files();
    const int stripWidth = _StripWidthSpin ? _StripWidthSpin->value() : 32;
    const int maxIterations = _MaxIterationsSpin ? _MaxIterationsSpin->value() : 10000;
    const double epsilon = _EpsilonSpin ? _EpsilonSpin->value() : 1.0;

    return [this, imageFiles, maskFiles, stripWidth, maxIterations, epsilon, globalSavePath]() {
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
            algorithm.SetStripWidth(stripWidth);
            algorithm.SetMaxIterations(maxIterations);
            algorithm.SetEpsilon(epsilon);
            PostLog(QString(">> 求解参数: 条带宽度=%1, 最大迭代次数=%2, 残差=%3")
                        .arg(stripWidth)
                        .arg(maxIterations)
                        .arg(epsilon, 0, 'g', 6));
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
