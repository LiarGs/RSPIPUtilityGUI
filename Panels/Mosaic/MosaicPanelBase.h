#pragma once

#include "Common/FileListWidget.h"
#include "Panels/AlgorithmPanelBase.h"

#include <QVBoxLayout>

namespace Panels::Mosaic {

using UI::Common::FileListWidget;

class MosaicPanelBase : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit MosaicPanelBase(QWidget *parent = nullptr);
    ~MosaicPanelBase() override = default;

    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;

  protected:
    void _SetupUi() override;

    FileListWidget *_ImageSelector = nullptr;
};

} // namespace Panels::Mosaic
