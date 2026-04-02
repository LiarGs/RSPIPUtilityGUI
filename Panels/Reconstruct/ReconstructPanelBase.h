#pragma once

#include <QVBoxLayout>

#include "Application/Execution/AlgorithmRequest.h"
#include "Common/FileSelectWidget.h"
#include "Panels/AlgorithmPanelBase.h"

namespace Panels::Reconstruct {

using UI::Common::FileSelectWidget;

class ReconstructPanelBase : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit ReconstructPanelBase(QWidget *parent = nullptr);
    ~ReconstructPanelBase() override = default;

    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;

  protected:
    void _SetupUi() override;
    void _PopulateSingleImageRequest(Application::Execution::SingleImageRequest &request,
                                     const QString &savePath) const;

    FileSelectWidget *_TargetSelect = nullptr;
    FileSelectWidget *_ReferSelect = nullptr;
    FileSelectWidget *_MaskSelect = nullptr;
};

} // namespace Panels::Reconstruct
