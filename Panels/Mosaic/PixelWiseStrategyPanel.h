#pragma once

#include "MosaicPanelBase.h"

#include <QComboBox>

class QGroupBox;

namespace Panels::Mosaic {

class PixelWiseStrategyPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit PixelWiseStrategyPanel(QWidget *parent = nullptr);

    QString AlgorithmDescription() const override;
    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  protected:
    void _SetupUi() override;

  private:
    void _UpdateMaskVisibility();
    bool _UsesAggregateMaskInputs() const;

    QComboBox *_StrategyCombo = nullptr;
    QGroupBox *_MaskGroup = nullptr;
    FileListWidget *_MaskSelector = nullptr;
};

} // namespace Panels::Mosaic
