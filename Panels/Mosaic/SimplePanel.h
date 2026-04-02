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
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Mosaic
