#include "ColorBalancePage.h"
#include "Common/FileSelectWidget.h" // [修改] 引入新控件头文件

#include <QComboBox>
#include <QDateTime>
#include <QFileInfo>
#include <QGroupBox>
#include <QMessageBox>
#include <QStandardPaths>
#include <QVBoxLayout>

// --- RSPIP Headers ---
#include "Algorithm/ColorBalance/MatchStatistic.h"
#include "Basic/GeoImage.h"
#include "Basic/Image.h"
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"

ColorBalancePage::ColorBalancePage(QWidget *parent) : IAlgorithmPage(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 1. 算法选择
    QGroupBox *algoGroup = new QGroupBox("1. 算法选择", this);
    QVBoxLayout *algoLayout = new QVBoxLayout(algoGroup);
    m_algoSelectCombo = new QComboBox(this);
    m_algoSelectCombo->addItem("MatchStatistics (统计匹配)", "MatchStatistics");
    algoLayout->addWidget(m_algoSelectCombo);
    layout->addWidget(algoGroup);

    // 2. 输入影像
    QGroupBox *inputGroup = new QGroupBox("2. 输入影像", this);
    QVBoxLayout *formLayout = new QVBoxLayout(inputGroup);
    formLayout->setSpacing(10); // 增加一点间距让界面更舒展

    // [修改] 使用 FileSelectWidget 替代原本繁琐的布局代码
    // 定义通用的过滤器
    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";

    m_targetSelect = new FileSelectWidget("目标影像 (Target):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    formLayout->addWidget(m_targetSelect);

    m_referSelect = new FileSelectWidget("参考影像 (Reference):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    formLayout->addWidget(m_referSelect);

    m_maskSelect = new FileSelectWidget("掩膜文件 (Mask, 可选):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    m_maskSelect->setPlaceholderText("留空则不使用掩膜...");
    formLayout->addWidget(m_maskSelect);

    layout->addWidget(inputGroup);
    layout->addStretch();
}

// [修改] execute 函数中获取路径的方式改变
void ColorBalancePage::execute(const QString &userSavePath) {
    // 使用 .currentPath() 获取路径
    QString targetPath = m_targetSelect->currentPath();
    QString referPath = m_referSelect->currentPath();
    QString maskPath = m_maskSelect->currentPath();
    QString algoType = m_algoSelectCombo->currentData().toString();

    // 1. 校验必填项
    if (targetPath.isEmpty() || referPath.isEmpty()) {
        QMessageBox::warning(this, "输入不完整", "请先选择目标影像和参考影像。");
        return;
    }

    emit logMessage(QString(">> [ColorBalance] 开始执行 [%1]...").arg(m_algoSelectCombo->currentText()));

    try {
        // ... (中间的读取逻辑和算法调用逻辑保持不变，为了节省篇幅略去) ...
        // ... 实际项目中请保留原有的 RSPIP 调用代码 ...

        // 模拟逻辑以演示编译通过
        emit logMessage(">> (重构演示) 正在读取数据...");
        // auto targetImg = RSPIP::IO::GeoImageRead(targetPath.toStdString());

        // ...

        // 假设算法执行完毕
        QString finalSavePath = userSavePath;
        if (finalSavePath.isEmpty()) {
            // 临时路径逻辑
            finalSavePath = "temp_result.tif";
        }

        emit logMessage(">> 处理完成 (模拟)");
        emit resultFileReady(finalSavePath);

    } catch (const std::exception &e) {
        emit logMessage(QString("异常: %1").arg(e.what()));
    }
}