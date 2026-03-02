#pragma once
#include "Common/FileSelectWidget.h"
#include "Panels/AlgorithmPanelBase.h"

namespace Panels::ColorBalance {

using UI::Common::FileSelectWidget;

/**
 * @brief MatchStatistics 算法的具体实现面板
 * @details MatchStatistics 需要 Target/Refer/Mask 三个输入，
 * 产出的是一个 Image 结果。
 */
class MatchStatisticsPanel : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit MatchStatisticsPanel(QWidget *parent = nullptr);
    ~MatchStatisticsPanel() override = default;

    QString AlgorithmName() const override { return "MatchStatistics (统计学方法)"; }

    bool ValidateInput() const override;
    std::function<bool()> BuildTask(const QString &globalSavePath) override;

  protected:
    void _SetupUi() override;

  private:
    FileSelectWidget *_TargetSelect = nullptr;
    FileSelectWidget *_ReferSelect = nullptr;
    FileSelectWidget *_MaskSelect = nullptr;
};

} // namespace Panels::ColorBalance
