#pragma once

#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QVBoxLayout>

#include "Algorithm/Reconstruct.h"
#include "Basic/Image.h"
#include "Common/FileSelectWidget.h"
#include "Panels/AlgorithmPanelBase.h"

namespace Panels::Reconstruct {

using UI::Common::FileSelectWidget;

/**
 * @brief 重构算法面板基类
 * @details 提取 Reconstruct 类算法共有的 UI 行为
 */
class ReconstructPanelBase : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit ReconstructPanelBase(QWidget *parent = nullptr);
    ~ReconstructPanelBase() override = default;
    bool ValidateInput() const override;

  protected:
    void _SetupUi() override;
    bool _SaveResult(const RSPIP::Image &result, const QString &userPath, const QString &prefix);

  protected:
    FileSelectWidget *_TargetSelect;
    FileSelectWidget *_ReferSelect;
    FileSelectWidget *_MaskSelect;
};

} // namespace Panels::Reconstruct
