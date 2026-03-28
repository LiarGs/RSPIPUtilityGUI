#pragma once

#include "Basic/Image.h"
#include "Common/FileSelectWidget.h"
#include "Panels/AlgorithmPanelBase.h"

#include <functional>

class QCheckBox;
class QVBoxLayout;

namespace Panels::Evaluation {

using UI::Common::FileSelectWidget;

/**
 * @brief 评估类算法面板公共基类
 * @details 统一提供两幅输入影像、可选掩膜、边界像素选项、基础校验与文本结果保存。
 */
class EvaluatorPanelBase : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit EvaluatorPanelBase(QWidget *parent = nullptr);
    ~EvaluatorPanelBase() override = default;

    bool ValidateInput() override;

  protected:
    void _SetupUi() override;

    bool _ValidatePairwiseCompatible(const RSPIP::Image &imageData,
                                     const RSPIP::Image &referenceImage);
    bool _ValidateMaskCompatible(const RSPIP::Image &imageData,
                                 const RSPIP::Image &maskImage);
    bool _SaveTextResult(const QString &content,
                         const QString &userPath,
                         const QString &prefix);
    bool _HasMaskInput() const;
    bool _IsBoundaryOnlyRequested() const;
    void _SetBoundaryOnlySupported(bool supported);

    FileSelectWidget *_ImageSelect = nullptr;
    FileSelectWidget *_ReferenceSelect = nullptr;
    FileSelectWidget *_MaskSelect = nullptr;
    QCheckBox *_BoundaryOnlyCheck = nullptr;
    QVBoxLayout *_MainLayout = nullptr;
    bool _BoundaryOnlySupported = false;

  private:
    void _UpdateBoundaryOnlyState();
};

} // namespace Panels::Evaluation
