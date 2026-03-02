#include "IsophotePanel.h"
#include <QApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
// --- RSPIP Headers ---
#include "Algorithm/ImageReconstruct/IsophoteConstrain.h"
#include "Basic/CloudMask.h"
#include "IO/ImageReader.h"

namespace Panels::Reconstruct {

IsophotePanel::IsophotePanel(QWidget *parent) : ReconstructPanelBase(parent) {
    _SetupUi();
}

void IsophotePanel::_SetupUi() {
    // 获取 BaseMosaicPanel 的布局
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        setLayout(layout);
    }

    // ============ 1. 算法参数设置组 ============
    auto *paramGroup = new QGroupBox("算法参数设置", this);
    auto *paramLayout = new QFormLayout(paramGroup);
    paramLayout->setContentsMargins(10, 15, 10, 10);
    paramLayout->setSpacing(8);

    // 最大迭代次数设置
    _MaxIterationsSpinBox = new QSpinBox(this);
    _MaxIterationsSpinBox->setRange(1, 10000); // 设置范围 1-10000
    _MaxIterationsSpinBox->setValue(1000);     // 设置默认值 1000
    _MaxIterationsSpinBox->setSingleStep(10);  // 步长 100
    _MaxIterationsSpinBox->setSuffix(" 次");   // 显示单位
    _MaxIterationsSpinBox->setToolTip("算法迭代的最大次数，值越大结果可能越精确但耗时越长");

    // 收敛阈值设置
    _EpsilonSpinBox = new QDoubleSpinBox(this);
    _EpsilonSpinBox->setRange(0.0001, 100.0); // 设置范围 0.0001-100
    _EpsilonSpinBox->setValue(1.0);           // 设置默认值 1.0
    _EpsilonSpinBox->setSingleStep(0.1);      // 步长 0.1
    _EpsilonSpinBox->setDecimals(4);          // 显示4位小数
    _EpsilonSpinBox->setToolTip("收敛阈值，当变化小于该值时停止迭代");

    // 添加到表单布局
    paramLayout->addRow("最大迭代次数:", _MaxIterationsSpinBox);
    paramLayout->addRow("收敛阈值 (ε):", _EpsilonSpinBox);

    layout->addWidget(paramGroup);
    layout->addStretch();
}

bool IsophotePanel::Run(const QString &globalSavePath) {

    emit LogMessage(">> 正在加载影像数据...");
    QApplication::processEvents();

    auto targetImage = RSPIP::IO::GeoImageRead(_TargetSelect->CurrentPath().toStdString());
    auto referImage = RSPIP::IO::GeoImageRead(_ReferSelect->CurrentPath().toStdString());
    auto maskImage = RSPIP::IO::CloudMaskImageRead(_MaskSelect->CurrentPath().toStdString());
    // 从 UI 控件读取当前值
    auto currentMaxIterations = _MaxIterationsSpinBox->value();
    auto currentEpsilon = _EpsilonSpinBox->value();

    if (!(targetImage && referImage && maskImage)) {
        emit LogMessage("错误: 无法读取影像或掩膜文件。");
        return false;
    }

    try {
        emit LogMessage(">> [IsophoteConstrain] 正在执行 (可能耗时较长)...");
        QApplication::processEvents();

        RSPIP::Algorithm::ReconstructAlgorithm::IsophoteConstrain algorithm(*targetImage, *referImage, *maskImage);
        algorithm.SetMaxIterations(currentMaxIterations);
        algorithm.SetEpsilon(currentEpsilon);
        algorithm.Execute();

        return _SaveResult(algorithm.AlgorithmResult, globalSavePath, "ReconstructIsophote");
    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }
}

} // namespace Panels::Reconstruct
