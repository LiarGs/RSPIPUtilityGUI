#pragma once
#include "IAlgorithmPage.h"

class QListWidget;
class QComboBox;
class QGroupBox;

class MosaicPage : public IAlgorithmPage {
    Q_OBJECT
public:
    explicit MosaicPage(QWidget *parent = nullptr);
    QString moduleName() const override { return "影像镶嵌 (Mosaic)"; }
    void execute(const QString &savePath) override;

private slots:
    void onAddImages();
    void onAddMasks();
    void onAlgoChanged(int index); // 处理算法切换时的UI变化

private:
    QComboBox *m_algoSelectCombo;

    // 影像输入区域
    QListWidget *m_imageList;

    // 掩膜输入区域 (仅 DynamicPatch 需要)
    QGroupBox *m_maskGroup;
    QListWidget *m_maskList;
};
