#include "RMSEEvaluatorPanel.h"

namespace Panels::Evaluation {

RMSEEvaluatorPanel::RMSEEvaluatorPanel(QWidget *parent)
    : EvaluatorPanelBase(parent) {
    _SetBoundaryOnlySupported(true);
}

QString RMSEEvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【RMSEEvaluator】\n\n"
        "基于均方根误差（RMSE）评价待评估影像与参考影像之间的整体误差。\n"
        "输入要求：两幅影像尺寸一致，可选传入 mask image 限制统计区域。\n"
        "当提供 mask 时，可进一步配置选区策略，并可选择仅统计 mask 边界像素。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
RMSEEvaluatorPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::RMSEEvaluationRequest>();
    _PopulateEvaluationRequest(*request, globalSavePath);
    return request;
}

} // namespace Panels::Evaluation
