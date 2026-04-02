#include "ColorBalancePanel.h"

namespace Panels::Reconstruct {

std::unique_ptr<Application::Execution::AlgorithmRequest>
ColorBalancePanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::ReconstructColorBalanceRequest>();
    _PopulateSingleImageRequest(*request, globalSavePath);
    return request;
}

} // namespace Panels::Reconstruct
