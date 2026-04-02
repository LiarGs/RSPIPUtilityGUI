#include "SSIMEvaluatorPanel.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>

namespace Panels::Evaluation {

SSIMEvaluatorPanel::SSIMEvaluatorPanel(QWidget *parent)
    : EvaluatorPanelBase(parent) {
    auto *paramGroup = new QGroupBox("算法参数", this);
    auto *formLayout = new QFormLayout(paramGroup);

    _K1Spin = new QDoubleSpinBox(paramGroup);
    _K1Spin->setDecimals(4);
    _K1Spin->setRange(0.0, 1.0);
    _K1Spin->setSingleStep(0.01);
    _K1Spin->setValue(0.01);
    formLayout->addRow("K1:", _K1Spin);

    _K2Spin = new QDoubleSpinBox(paramGroup);
    _K2Spin->setDecimals(4);
    _K2Spin->setRange(0.0, 1.0);
    _K2Spin->setSingleStep(0.01);
    _K2Spin->setValue(0.03);
    formLayout->addRow("K2:", _K2Spin);

    _SetBoundaryOnlySupported(true);

    if (_MainLayout) {
        const int insertIndex = _MainLayout->count() > 0 ? _MainLayout->count() - 1 : 0;
        _MainLayout->insertWidget(insertIndex, paramGroup);
    }
}

QString SSIMEvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【SSIMEvaluator】\n\n"
        "基于结构相似性指数（SSIM）衡量待评估影像与参考影像在亮度、对比度和结构上的一致性。\n"
        "输入要求：两幅影像尺寸一致，可选传入 mask image 限制统计区域。\n"
        "参数说明：K1 与 K2 默认分别为 0.01 与 0.03；当提供 mask 时，可进一步配置选区策略，"
        "并可选择仅统计 mask 边界像素。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
SSIMEvaluatorPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::SSIMEvaluationRequest>();
    _PopulateEvaluationRequest(*request, globalSavePath);
    request->K1 = _K1Spin ? _K1Spin->value() : 0.01;
    request->K2 = _K2Spin ? _K2Spin->value() : 0.03;
    return request;
}

} // namespace Panels::Evaluation
