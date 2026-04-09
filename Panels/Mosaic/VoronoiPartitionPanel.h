#pragma once

#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

class VoronoiPartitionPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit VoronoiPartitionPanel(QWidget *parent = nullptr);

    QString AlgorithmDescription() const override;
    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;
    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_MaskSelector = nullptr;
};

} // namespace Panels::Mosaic
