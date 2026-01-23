#include "ReconstructPage.h"
#include "Common/FileSelectWidget.h" // 引入通用控件

#include <QComboBox>
#include <QDateTime>
#include <QFileInfo>
#include <QGroupBox>
#include <QMessageBox>
#include <QStandardPaths>
#include <QVBoxLayout>

// --- RSPIP Headers ---
#include "Algorithm/ImageReconstruct/ColorBalanceReconstruct.h"
#include "Algorithm/ImageReconstruct/IsophoteConstrain.h"
#include "Algorithm/ImageReconstruct/Simple.h"
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"

ReconstructPage::ReconstructPage(QWidget *parent) : IAlgorithmPage(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 1. 算法选择
    QGroupBox *algoGroup = new QGroupBox("1. 算法选择", this);
    QVBoxLayout *algoLayout = new QVBoxLayout(algoGroup);
    m_algoSelectCombo = new QComboBox(this);
    m_algoSelectCombo->addItem("IsophoteConstrain (等照度线约束)", "Isophote");
    m_algoSelectCombo->addItem("ColorBalanceReconstruct (匀色重构)", "ColorBalance");
    m_algoSelectCombo->addItem("Simple (简单的Mask复制)", "Simple");
    algoLayout->addWidget(m_algoSelectCombo);
    layout->addWidget(algoGroup);

    // 2. 输入文件路径
    QGroupBox *inputGroup = new QGroupBox("2. 输入文件路径", this);
    QVBoxLayout *formLayout = new QVBoxLayout(inputGroup);
    formLayout->setSpacing(10);

    // 定义过滤器
    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg)";

    // 使用 FileSelectWidget 极大地简化了 UI 构建代码
    m_targetSelect = new FileSelectWidget("待修复影像 (Target):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    formLayout->addWidget(m_targetSelect);

    m_referSelect = new FileSelectWidget("参考影像 (Reference):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    formLayout->addWidget(m_referSelect);

    m_maskSelect = new FileSelectWidget("云掩膜 (Mask):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    formLayout->addWidget(m_maskSelect);

    layout->addWidget(inputGroup);
    layout->addStretch();
}

void ReconstructPage::execute(const QString &userSavePath) {
    QString algoType = m_algoSelectCombo->currentData().toString();

    // 获取路径的新方式
    QString targetPath = m_targetSelect->currentPath();
    QString referPath = m_referSelect->currentPath();
    QString maskPath = m_maskSelect->currentPath();

    emit logMessage(QString(">> [Reconstruct] 开始执行 [%1]...").arg(m_algoSelectCombo->currentText()));

    // 校验输入
    if (targetPath.isEmpty() || referPath.isEmpty() || maskPath.isEmpty()) {
        QMessageBox::warning(this, "输入不完整", "请补全 Target, Reference 和 Mask 三个文件路径。");
        return;
    }

    try {
        // 1. 读取数据
        emit logMessage(">> 正在加载影像数据...");
        auto targetImg = RSPIP::IO::GeoImageRead(targetPath.toStdString());
        auto referImg = RSPIP::IO::GeoImageRead(referPath.toStdString());
        auto maskImg = RSPIP::IO::CloudMaskImageRead(maskPath.toStdString());

        if (!targetImg || !referImg || !maskImg) {
            emit logMessage("错误: 影像或掩膜文件读取失败。");
            return;
        }

        // 2. 准备算法 (保持原有逻辑)
        std::unique_ptr<RSPIP::Algorithm::ReconstructAlgorithm::ReconstructAlgorithmBase> algorithm;

        if (algoType == "Isophote") {
            algorithm = std::make_unique<RSPIP::Algorithm::ReconstructAlgorithm::IsophoteConstrain>(*targetImg, *referImg, *maskImg);
        } else if (algoType == "ColorBalance") {
            algorithm = std::make_unique<RSPIP::Algorithm::ReconstructAlgorithm::ColorBalanceReconstruct>(*targetImg, *referImg, *maskImg);
        } else if (algoType == "Simple") {
            algorithm = std::make_unique<RSPIP::Algorithm::ReconstructAlgorithm::Simple>(*targetImg, *referImg, *maskImg);
        }

        // 3. 执行
        if (algorithm) {
            emit logMessage(">> 算法计算中 (可能需要较长时间)...");
            algorithm->Execute();

            // 4. 路径处理
            QString finalSavePath = userSavePath;
            if (finalSavePath.isEmpty()) {
                QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
                finalSavePath = QString("%1/reconstruct_result_%2.tif")
                                    .arg(tempDir)
                                    .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
                emit logMessage(QString(">> 使用临时路径: %1").arg(finalSavePath));
            }

            // 5. 保存
            QFileInfo saveInfo(finalSavePath);
            bool saved = RSPIP::IO::SaveImage(algorithm->AlgorithmResult,
                                              saveInfo.absolutePath().toStdString(),
                                              saveInfo.fileName().toStdString());

            if (saved) {
                emit logMessage(">> 重构完成并保存！");
                emit resultFileReady(finalSavePath);
            } else {
                emit logMessage("错误: 结果保存失败。");
            }
        }

    } catch (const std::exception &e) {
        emit logMessage(QString("异常: %1").arg(e.what()));
    }
}