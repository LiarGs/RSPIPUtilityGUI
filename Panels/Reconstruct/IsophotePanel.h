#pragma once
#include "ReconstructPanelBase.h"
#include <QDoubleSpinBox>
#include <QSpinBox>

namespace Panels::Reconstruct {

/**
 * @brief IsophoteConstrain (等照度线约束) 算法面板
 */
class IsophotePanel : public ReconstructPanelBase {
    Q_OBJECT
  public:
    explicit IsophotePanel(QWidget *parent = nullptr);
    bool Run(const QString &globalSavePath) override;
    QString AlgorithmName() const override { return "IsophoteConstrain (等照度线约束)"; }

  protected:
    void _SetupUi() override;

  private:
    QSpinBox *_MaxIterationsSpinBox = nullptr;
    QDoubleSpinBox *_EpsilonSpinBox = nullptr;
};

} // namespace Panels::Reconstruct
