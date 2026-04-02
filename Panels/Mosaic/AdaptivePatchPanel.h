#pragma once
#include "MosaicPanelBase.h"
#include <QSpinBox>

namespace Panels::Mosaic {

/**
 * @brief AdaptivePatch (自适应补丁) 算法面板.
 * @details 该算法除了影像列表外，还需要掩膜列表.
 */
class AdaptivePatchPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit AdaptivePatchPanel(QWidget *parent = nullptr);

    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_MaskSelector = nullptr;
    QSpinBox *_StripWidthSpin = nullptr;
};

} // namespace Panels::Mosaic
