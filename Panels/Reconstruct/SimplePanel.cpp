#include "SimplePanel.h"

namespace Panels::Reconstruct {

std::unique_ptr<Application::Execution::AlgorithmRequest>
SimplePanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::ReconstructSimpleRequest>();
    _PopulateSingleImageRequest(*request, globalSavePath);
    return request;
}

} // namespace Panels::Reconstruct
