#include "PSNREvaluatorPanel.h"

namespace Panels::Evaluation {

QString PSNREvaluatorPanel::AlgorithmDescription() const {
    return QStringLiteral(
        "【PSNREvaluator】\n\n"
        "基于峰值信噪比（PSNR）评价待评估影像与参考影像之间的整体误差。\n"
        "输入要求：两幅影像尺寸一致、波段数一致；可选通过 mask image 限制统计区域。\n"
        "当提供 mask 时，可进一步配置 Band、选区模式和 SelectedValues。\n"
        "输出形式：在日志中显示评估值，并保存为文本结果文件。");
}

std::unique_ptr<Application::Execution::AlgorithmRequest>
PSNREvaluatorPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::PSNREvaluationRequest>();
    _PopulateEvaluationRequest(*request, globalSavePath);
    return request;
}

} // namespace Panels::Evaluation
