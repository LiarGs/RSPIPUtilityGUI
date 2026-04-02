#pragma once

#include "Common/FileListWidget.h"
#include "Panels/AlgorithmPanelBase.h"

namespace Panels::Preprocess {

using UI::Common::FileListWidget;

class GeoCoordinateAlignPanel : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit GeoCoordinateAlignPanel(QWidget *parent = nullptr);
    ~GeoCoordinateAlignPanel() override = default;

    QString AlgorithmDescription() const override;
    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_ImageSelector = nullptr;
    FileListWidget *_MaskSelector = nullptr;
};

} // namespace Panels::Preprocess
