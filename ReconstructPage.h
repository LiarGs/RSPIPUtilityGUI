#pragma once
#include "IAlgorithmPage.h"

class QComboBox;
class FileSelectWidget; // 前置声明通用控件

class ReconstructPage : public IAlgorithmPage {
    Q_OBJECT
  public:
    explicit ReconstructPage(QWidget *parent = nullptr);
    QString moduleName() const override { return "影像重构 (Reconstruct)"; }
    void execute(const QString &savePath) override;

    // 已删除冗余的 onBrowse... 槽函数

  private:
    QComboBox *m_algoSelectCombo;

    // 使用通用控件替换 QLineEdit
    FileSelectWidget *m_targetSelect;
    FileSelectWidget *m_referSelect;
    FileSelectWidget *m_maskSelect;
};