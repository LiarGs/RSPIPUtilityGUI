#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

/**
 * @brief ShowOverLap (显示重叠区域) 算法面板
 */
class ShowOverlapPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit ShowOverlapPanel(QWidget *parent = nullptr) : MosaicPanelBase(parent) {}
    QString AlgorithmName() const override { return "ShowOverLap (显示重叠区域)"; }
    bool Run(const QString &globalSavePath) override;
};

} // namespace Panels::Mosaic
