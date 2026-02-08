#include "MosaicPanelBase.h"

namespace Panels::Mosaic {

/**
 * @brief DynamicPatch (动态补丁去云) 算法面板
 * @details 该算法除了影像列表外，还需要掩膜列表
 */
class DynamicPatchPanel : public MosaicPanelBase {
    Q_OBJECT
  public:
    explicit DynamicPatchPanel(QWidget *parent = nullptr);

    QString AlgorithmName() const override { return "DynamicPatch (动态补丁去云)"; }
    bool ValidateInput() const override;
    bool Run(const QString &globalSavePath) override;

  protected:
    void _SetupUi() override;

  private:
    FileListWidget *_MaskSelector;
};

} // namespace Panels::Mosaic