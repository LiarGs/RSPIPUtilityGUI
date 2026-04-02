#include "SimplePanel.h"

namespace Panels::Mosaic {

std::unique_ptr<Application::Execution::AlgorithmRequest>
SimplePanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MosaicSimpleRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    return request;
}

} // namespace Panels::Mosaic
