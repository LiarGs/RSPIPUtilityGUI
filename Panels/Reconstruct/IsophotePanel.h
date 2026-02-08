#pragma once
#include "ReconstructPanelBase.h"

namespace Panels::Reconstruct {

/**
 * @brief IsophoteConstrain (等照度线约束) 算法面板
 */
class IsophotePanel : public ReconstructPanelBase {
    Q_OBJECT
  public:
    explicit IsophotePanel(QWidget *parent = nullptr) : ReconstructPanelBase(parent) {}
    bool Run(const QString &globalSavePath) override;
    QString AlgorithmName() const override { return "IsophoteConstrain (等照度线约束)"; }
};

} // namespace Panels::Reconstruct