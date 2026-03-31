#pragma once
#include "MosaicPanelBase.h"
#include <QSpinBox>

namespace Panels::Mosaic {

/**
 * @brief AdaptiveColorBalancePatch (自适应匀色补丁) 算法面板.
 * @details 该算法除了影像列表外，还需要掩膜列表.
 */
class AdaptiveColorBalancePatchPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit AdaptiveColorBalancePatchPanel(QWidget *parent = nullptr);

    QString AlgorithmName() const override { return "AdaptiveColorBalancePatch (自适应匀色补丁)"; }
    bool ValidateInput() override;
    std::function<bool()> BuildTask(const QString &globalSavePath) override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_MaskSelector = nullptr;
    QSpinBox *_StripWidthSpin = nullptr;
};

} // namespace Panels::Mosaic
