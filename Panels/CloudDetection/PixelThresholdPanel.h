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

    QString AlgorithmName() const override { return "PixelThreshold (像素阈值法)"; }
    OutputSelectionMode PreferredOutputSelectionMode() const override {
        return OutputSelectionMode::Directory;
    }
    QString AlgorithmDescription() const override;
    bool ValidateInput() override;
    std::function<bool()> BuildTask(const QString &globalSavePath) override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_InputSelector = nullptr;
    QComboBox *_ModeCombo = nullptr;
    QSpinBox *_ThresholdSpin = nullptr;
};

} // namespace Panels::CloudDetection
