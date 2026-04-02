#pragma once

#include "Common/FileListWidget.h"
#include "Panels/AlgorithmPanelBase.h"
#include <QComboBox>
#include <QSpinBox>

namespace Panels::CloudDetection {

using UI::Common::FileListWidget;

class PixelThresholdPanel : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit PixelThresholdPanel(QWidget *parent = nullptr);
    ~PixelThresholdPanel() override = default;

    QString AlgorithmDescription() const override;
    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_InputSelector = nullptr;
    QComboBox *_ModeCombo = nullptr;
    QSpinBox *_ThresholdSpin = nullptr;
};

} // namespace Panels::CloudDetection
