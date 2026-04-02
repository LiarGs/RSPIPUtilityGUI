#pragma once
#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

/**
 * @brief ShowOverLap (显示重叠区域) 算法面板
 */
class ShowOverlapPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit ShowOverlapPanel(QWidget *parent = nullptr) : MosaicPanelBase(parent) {}
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Mosaic
