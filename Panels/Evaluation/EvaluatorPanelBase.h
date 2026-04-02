#pragma once

#include "Application/Execution/AlgorithmRequest.h"
#include "Common/FileSelectWidget.h"
#include "Panels/AlgorithmPanelBase.h"

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class QVBoxLayout;

namespace Panels::Evaluation {

using UI::Common::FileSelectWidget;

class EvaluatorPanelBase : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit EvaluatorPanelBase(QWidget *parent = nullptr);
    ~EvaluatorPanelBase() override = default;

    std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() override;

  protected:
    void _SetupUi() override;
    void _PopulateEvaluationRequest(Application::Execution::EvaluationRequest &request,
                                    const QString &savePath) const;
    bool _HasMaskInput() const;
    bool _IsBoundaryOnlyRequested() const;
    void _SetBoundaryOnlySupported(bool supported);
    void _SetMaskRequired(bool required);

    FileSelectWidget *_ImageSelect = nullptr;
    FileSelectWidget *_ReferenceSelect = nullptr;
    FileSelectWidget *_MaskSelect = nullptr;
    QGroupBox *_MaskPolicyGroup = nullptr;
    QSpinBox *_MaskBandSpin = nullptr;
    QComboBox *_MaskModeCombo = nullptr;
    QLineEdit *_SelectedValuesEdit = nullptr;
    QCheckBox *_BoundaryOnlyCheck = nullptr;
    QVBoxLayout *_MainLayout = nullptr;
    bool _BoundaryOnlySupported = false;
    bool _MaskRequired = false;

  private:
    bool _ValidateMaskPolicyInputSyntax() const;
    void _UpdateMaskPolicyState();
    void _UpdateBoundaryOnlyState();
};

} // namespace Panels::Evaluation
