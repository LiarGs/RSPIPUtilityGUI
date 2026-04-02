#include "BoundaryGradientEvaluatorPanel.h"

namespace Panels::Evaluation {

BoundaryGradientEvaluatorPanel::BoundaryGradientEvaluatorPanel(QWidget *parent)
    : EvaluatorPanelBase(parent) {
    _SetMaskRequired(true);
    if (_MaskSelect) {
        _MaskSelect->SetPlaceholderText("BoundaryGradientEvaluator 必须提供掩膜文件...");
    }
}

QString BoundaryGradientEvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【BoundaryGradientEvaluator】\n\n"
        "基于边界梯度评价待评估影像与参考影像在掩膜边界附近的过渡质量。\n"
        "输入要求：两幅影像尺寸一致，且必须提供 mask image。\n"
        "当提供 mask 时，可进一步配置 Band、选区模式和 SelectedValues。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
BoundaryGradientEvaluatorPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::BoundaryGradientEvaluationRequest>();
    _PopulateEvaluationRequest(*request, globalSavePath);
    return request;
}

} // namespace Panels::Evaluation
