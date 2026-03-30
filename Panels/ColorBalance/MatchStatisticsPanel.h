#pragma once
#include "Common/FileListWidget.h"
#include "Common/FileSelectWidget.h"
#include "Panels/AlgorithmPanelBase.h"

namespace Panels::ColorBalance {

using UI::Common::FileListWidget;
using UI::Common::FileSelectWidget;

/**
 * @brief MatchStatistics 算法的具体实现面板
 * @details 支持单张基准图 + 多张待匀色影像 + 可选对应掩膜的一对一批处理。
 */
class MatchStatisticsPanel : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit MatchStatisticsPanel(QWidget *parent = nullptr);
    ~MatchStatisticsPanel() override = default;

    QString AlgorithmName() const override { return "MatchStatistics (统计学方法)"; }
    OutputSelectionMode PreferredOutputSelectionMode() const override {
        return OutputSelectionMode::Directory;
    }

    bool ValidateInput() override;
    std::function<bool()> BuildTask(const QString &globalSavePath) override;

  protected:
    void _SetupUi() override;

  private:
    FileSelectWidget *_ReferSelect = nullptr;
    FileListWidget *_InputSelector = nullptr;
    FileListWidget *_MaskSelector = nullptr;
};

} // namespace Panels::ColorBalance

