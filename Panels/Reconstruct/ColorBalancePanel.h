#pragma once
#include "ReconstructPanelBase.h"

namespace Panels::Reconstruct {

/**
 * @brief ColorBalanceReconstruct (匀色重构) 算法面板
 */
class ColorBalancePanel : public ReconstructPanelBase {
    Q_OBJECT
  public:
    explicit ColorBalancePanel(QWidget *parent = nullptr) : ReconstructPanelBase(parent) {}
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Reconstruct
