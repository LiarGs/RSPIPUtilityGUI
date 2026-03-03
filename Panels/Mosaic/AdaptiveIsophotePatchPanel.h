#pragma once
#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

/**
 * @brief AdaptiveIsophotePatch (等照度自适应补丁) 算法面板
 * @details 该算法除了影像列表外，还需要掩膜列表
 */
class AdaptiveIsophotePatchPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit AdaptiveIsophotePatchPanel(QWidget *parent = nullptr);

    QString AlgorithmName() const override { return "AdaptiveIsophotePatch (等照度自适应补丁)"; }
    bool ValidateInput() override;
    std::function<bool()> BuildTask(const QString &globalSavePath) override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_MaskSelector = nullptr;
};

} // namespace Panels::Mosaic

