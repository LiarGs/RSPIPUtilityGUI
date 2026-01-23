#pragma once
#include "IAlgorithmPage.h"

class QComboBox;
class QGroupBox;
class FileListWidget; // 引入新控件前置声明

class MosaicPage : public IAlgorithmPage {
    Q_OBJECT
  public:
    explicit MosaicPage(QWidget *parent = nullptr);
    QString moduleName() const override { return "影像镶嵌 (Mosaic)"; }
    void execute(const QString &savePath) override;

  private slots:
    void onAlgoChanged(int index);

  private:
    QComboBox *m_algoSelectCombo;

    // 使用 FileListWidget 替换原始的 QListWidget + Button 组合
    FileListWidget *m_imageSelector;

    // 掩膜输入部分
    QGroupBox *m_maskGroup;
    FileListWidget *m_maskSelector;
};