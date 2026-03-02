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
    std::function<bool()> BuildTask(const QString &globalSavePath) override;
    QString AlgorithmName() const override { return "Simple (简单的Mask复制)"; }
};

} // namespace Panels::Reconstruct
