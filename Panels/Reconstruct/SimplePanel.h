#pragma once
#include "ReconstructPanelBase.h"

namespace Panels::Reconstruct {

/**
 * @brief Simple (简单的Mask复制) 算法面板
 */
class SimplePanel : public ReconstructPanelBase {
    Q_OBJECT
  public:
    explicit SimplePanel(QWidget *parent = nullptr) : ReconstructPanelBase(parent) {}
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Reconstruct
