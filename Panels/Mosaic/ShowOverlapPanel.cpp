#include "ShowOverlapPanel.h"

namespace Panels::Mosaic {

std::unique_ptr<Application::Execution::AlgorithmRequest>
ShowOverlapPanel::CollectRequest(const QString &globalSavePath) const {
    auto request = std::make_unique<Application::Execution::MosaicShowOverlapRequest>();
    request->SavePath = globalSavePath.trimmed();
    request->ImageFiles = _ImageSelector ? _ImageSelector->Files() : QStringList();
    return request;
}

} // namespace Panels::Mosaic
