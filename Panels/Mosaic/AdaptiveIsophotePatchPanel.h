#pragma once
#include "MosaicPanelBase.h"
#include <QDoubleSpinBox>
#include <QSpinBox>

namespace Panels::Mosaic {

/**
 * @brief AdaptiveIsophotePatch (等照度自适应补丁) 算法面板
 * @details 该算法除了影像列表外，还需要掩膜列表
 */
class AdaptiveIsophotePatchPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit AdaptiveIsophotePatchPanel(QWidget *parent = nullptr);

    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_MaskSelector = nullptr;
    QSpinBox *_StripWidthSpin = nullptr;
    QSpinBox *_MaxIterationsSpin = nullptr;
    QDoubleSpinBox *_EpsilonSpin = nullptr;
};

} // namespace Panels::Mosaic
