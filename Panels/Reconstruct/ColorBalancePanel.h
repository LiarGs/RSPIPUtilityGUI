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
    QString AlgorithmName() const override { return "ColorBalanceReconstruct (匀色重构)"; }
    bool Run(const QString &globalSavePath) override;
};

} // namespace Panels::Reconstruct