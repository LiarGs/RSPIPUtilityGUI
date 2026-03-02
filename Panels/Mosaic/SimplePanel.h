#pragma once
#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

/**
 * @brief Simple (直接覆盖) 算法面板
 */
class SimplePanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit SimplePanel(QWidget *parent = nullptr) : MosaicPanelBase(parent) {}
    QString AlgorithmName() const override { return "Simple (直接覆盖)"; }
    std::function<bool()> BuildTask(const QString &globalSavePath) override;
};

} // namespace Panels::Mosaic
