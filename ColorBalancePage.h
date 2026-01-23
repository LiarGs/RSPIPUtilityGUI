#pragma once
#include "IAlgorithmPage.h"

// 前置声明
class QComboBox;
class FileSelectWidget; // [修改] 引入新类的前置声明

class ColorBalancePage : public IAlgorithmPage {
    Q_OBJECT
  public:
    explicit ColorBalancePage(QWidget *parent = nullptr);
    QString moduleName() const override { return "匀色处理 (Color Balance)"; }

    void execute(const QString &savePath) override;

    // [修改] 删除了 onBrowseTarget, onBrowseRefer, onBrowseMask 槽函数
    // 因为这些逻辑现在由 FileSelectWidget 内部处理

  private:
    QComboBox *m_algoSelectCombo;

    // [修改] 将 QLineEdit* 替换为 FileSelectWidget*
    FileSelectWidget *m_targetSelect;
    FileSelectWidget *m_referSelect;
    FileSelectWidget *m_maskSelect;
};