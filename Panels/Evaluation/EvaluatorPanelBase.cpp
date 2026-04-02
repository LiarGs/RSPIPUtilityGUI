#include "EvaluatorPanelBase.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QStringList>
#include <QVBoxLayout>

namespace Panels::Evaluation {

EvaluatorPanelBase::EvaluatorPanelBase(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void EvaluatorPanelBase::_SetupUi() {
    _MainLayout = new QVBoxLayout(this);
    _MainLayout->setContentsMargins(0, 0, 0, 0);
    _MainLayout->setSpacing(10);

    const QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.jpeg *.bmp)";

    _ImageSelect = new FileSelectWidget("待评估影像 (Image):",
                                        imgFilter,
                                        FileSelectWidget::Mode::FileOpen,
                                        this);
    _MainLayout->addWidget(_ImageSelect);

    _ReferenceSelect = new FileSelectWidget("参考影像 (Reference):",
                                            imgFilter,
                                            FileSelectWidget::Mode::FileOpen,
                                            this);
    _MainLayout->addWidget(_ReferenceSelect);

    _MaskSelect = new FileSelectWidget("掩膜影像 (Mask, 可选):",
                                       imgFilter,
                                       FileSelectWidget::Mode::FileOpen,
                                       this);
    _MaskSelect->SetPlaceholderText("留空则对整幅影像计算...");
    _MainLayout->addWidget(_MaskSelect);

    _MaskPolicyGroup = new QGroupBox("Mask 选区策略", this);
    auto *maskPolicyLayout = new QFormLayout(_MaskPolicyGroup);

    _MaskBandSpin = new QSpinBox(_MaskPolicyGroup);
    _MaskBandSpin->setRange(1, 1024);
    _MaskBandSpin->setValue(1);
    maskPolicyLayout->addRow("Band:", _MaskBandSpin);

    _MaskModeCombo = new QComboBox(_MaskPolicyGroup);
    _MaskModeCombo->addItem("NonZeroSelected", static_cast<int>(Application::Execution::MaskSelectionMode::NonZeroSelected));
    _MaskModeCombo->addItem("ValueSetSelected", static_cast<int>(Application::Execution::MaskSelectionMode::ValueSetSelected));
    maskPolicyLayout->addRow("Mode:", _MaskModeCombo);

    _SelectedValuesEdit = new QLineEdit(_MaskPolicyGroup);
    _SelectedValuesEdit->setPlaceholderText("例如: 1, 255");
    maskPolicyLayout->addRow("SelectedValues:", _SelectedValuesEdit);

    _MaskPolicyGroup->setVisible(false);
    _MainLayout->addWidget(_MaskPolicyGroup);

    _BoundaryOnlyCheck = new QCheckBox("仅计算 Mask 边界像素", this);
    _BoundaryOnlyCheck->setVisible(false);
    _BoundaryOnlyCheck->setEnabled(false);
    _MainLayout->addWidget(_BoundaryOnlyCheck);

    connect(_MaskSelect, &FileSelectWidget::PathChanged, this,
            [this](const QString &) {
                _UpdateMaskPolicyState();
                _UpdateBoundaryOnlyState();
            });
    connect(_MaskModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { _UpdateMaskPolicyState(); });

    _UpdateMaskPolicyState();
    _UpdateBoundaryOnlyState();

    _MainLayout->addStretch();
}

std::optional<Infrastructure::Execution::ValidationIssue> EvaluatorPanelBase::ValidateInput() {
    if (_ImageSelect->CurrentPath().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请选择待评估影像"};
    }

    if (_ReferenceSelect->CurrentPath().isEmpty()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "请选择参考影像"};
    }

    if (_MaskRequired && !_HasMaskInput()) {
        return Infrastructure::Execution::ValidationIssue{"输入错误", "当前算法必须提供掩膜文件"};
    }

    if (!_ValidateMaskPolicyInputSyntax()) {
        return Infrastructure::Execution::ValidationIssue{
            "输入错误",
            "ValueSetSelected 模式下必须提供逗号分隔的整数 SelectedValues。"};
    }

    if (_IsBoundaryOnlyRequested() && !_HasMaskInput()) {
        return Infrastructure::Execution::ValidationIssue{
            "输入错误",
            "仅在提供掩膜文件后才能勾选仅计算边界像素"};
    }

    return std::nullopt;
}

void EvaluatorPanelBase::_PopulateEvaluationRequest(Application::Execution::EvaluationRequest &request,
                                                    const QString &savePath) const {
    request.SavePath = savePath.trimmed();
    request.ImagePath = _ImageSelect ? _ImageSelect->CurrentPath() : QString();
    request.ReferencePath = _ReferenceSelect ? _ReferenceSelect->CurrentPath() : QString();
    request.MaskPath = _MaskSelect ? _MaskSelect->CurrentPath() : QString();
    request.BoundaryOnly = _IsBoundaryOnlyRequested();

    if (!_HasMaskInput()) {
        request.MaskPolicy = std::nullopt;
        return;
    }

    Application::Execution::MaskSelectionPolicyRequest policy;
    policy.Band = _MaskBandSpin ? _MaskBandSpin->value() : 1;
    policy.Mode = (_MaskModeCombo &&
                   _MaskModeCombo->currentData().toInt() ==
                       static_cast<int>(Application::Execution::MaskSelectionMode::ValueSetSelected))
                      ? Application::Execution::MaskSelectionMode::ValueSetSelected
                      : Application::Execution::MaskSelectionMode::NonZeroSelected;

    if (policy.Mode == Application::Execution::MaskSelectionMode::ValueSetSelected && _SelectedValuesEdit) {
        const QStringList tokens = _SelectedValuesEdit->text().split(',', Qt::SkipEmptyParts);
        for (const QString &token : tokens) {
            bool ok = false;
            const int value = token.trimmed().toInt(&ok);
            if (ok) {
                policy.SelectedValues.push_back(value);
            }
        }
    }

    request.MaskPolicy = policy;
}

bool EvaluatorPanelBase::_HasMaskInput() const {
    return _MaskSelect && !_MaskSelect->CurrentPath().trimmed().isEmpty();
}

bool EvaluatorPanelBase::_IsBoundaryOnlyRequested() const {
    return _BoundaryOnlyCheck && _BoundaryOnlyCheck->isVisible() &&
           _BoundaryOnlyCheck->isEnabled() && _BoundaryOnlyCheck->isChecked();
}

void EvaluatorPanelBase::_SetBoundaryOnlySupported(bool supported) {
    _BoundaryOnlySupported = supported;
    _UpdateBoundaryOnlyState();
}

void EvaluatorPanelBase::_SetMaskRequired(bool required) {
    _MaskRequired = required;
}

bool EvaluatorPanelBase::_ValidateMaskPolicyInputSyntax() const {
    if (!_HasMaskInput()) {
        return true;
    }

    const auto mode = _MaskModeCombo &&
                              _MaskModeCombo->currentData().toInt() ==
                                  static_cast<int>(Application::Execution::MaskSelectionMode::ValueSetSelected)
                          ? Application::Execution::MaskSelectionMode::ValueSetSelected
                          : Application::Execution::MaskSelectionMode::NonZeroSelected;
    if (mode != Application::Execution::MaskSelectionMode::ValueSetSelected) {
        return true;
    }

    const QStringList tokens = _SelectedValuesEdit
                                   ? _SelectedValuesEdit->text().split(',', Qt::SkipEmptyParts)
                                   : QStringList();
    if (tokens.isEmpty()) {
        return false;
    }

    for (const QString &token : tokens) {
        bool ok = false;
        token.trimmed().toInt(&ok);
        if (!ok) {
            return false;
        }
    }
    return true;
}

void EvaluatorPanelBase::_UpdateMaskPolicyState() {
    if (!_MaskPolicyGroup || !_MaskModeCombo || !_SelectedValuesEdit) {
        return;
    }

    const bool hasMask = _HasMaskInput();
    _MaskPolicyGroup->setVisible(hasMask);
    _MaskPolicyGroup->setEnabled(hasMask);

    const auto mode = static_cast<Application::Execution::MaskSelectionMode>(_MaskModeCombo->currentData().toInt());
    const bool enableSelectedValues = hasMask && mode == Application::Execution::MaskSelectionMode::ValueSetSelected;
    _SelectedValuesEdit->setEnabled(enableSelectedValues);

    if (!enableSelectedValues) {
        _SelectedValuesEdit->clear();
    }
}

void EvaluatorPanelBase::_UpdateBoundaryOnlyState() {
    if (!_BoundaryOnlyCheck) {
        return;
    }

    _BoundaryOnlyCheck->setVisible(_BoundaryOnlySupported);

    const bool enableBoundaryOnly = _BoundaryOnlySupported && _HasMaskInput();
    _BoundaryOnlyCheck->setEnabled(enableBoundaryOnly);

    if (!enableBoundaryOnly) {
        _BoundaryOnlyCheck->setChecked(false);
    }
}

} // namespace Panels::Evaluation
