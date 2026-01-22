#include "ReconstructPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDateTime>

// --- RSPIP Headers ---
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"
#include "Algorithm/ImageReconstruct/Simple.h"
#include "Algorithm/ImageReconstruct/IsophoteConstrain.h"
#include "Algorithm/ImageReconstruct/ColorBalanceReconstruct.h"

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

    // 2. 输入参数
    QGroupBox *inputGroup = new QGroupBox("2. 输入文件路径", this);
    QVBoxLayout *formLayout = new QVBoxLayout(inputGroup);

    auto createRow = [this](const QString &title, QLineEdit *&edit, auto slot) {
        QLabel *lbl = new QLabel(title, this);
        edit = new QLineEdit(this);
        QPushButton *btn = new QPushButton("浏览...", this);
        btn->setFixedWidth(60);
        connect(btn, &QPushButton::clicked, this, slot);
        QHBoxLayout *hBox = new QHBoxLayout();
        hBox->addWidget(edit);
        hBox->addWidget(btn);
        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->addWidget(lbl);
        vBox->addLayout(hBox);
        return vBox;
    };

    formLayout->addLayout(createRow("待修复影像 (Target):", m_targetEdit, &ReconstructPage::onBrowseTarget));
    formLayout->addLayout(createRow("参考影像 (Reference):", m_referEdit, &ReconstructPage::onBrowseRefer));
    formLayout->addLayout(createRow("云掩膜 (Mask):", m_maskEdit, &ReconstructPage::onBrowseMask));

    layout->addWidget(inputGroup);
    layout->addStretch();
}

void ReconstructPage::onBrowseTarget() {
    QString f = QFileDialog::getOpenFileName(this, "选择待修复影像", "", "Images (*.tif *.tiff *.png *.jpg)");
    if (!f.isEmpty()) m_targetEdit->setText(f);
}
void ReconstructPage::onBrowseRefer() {
    QString f = QFileDialog::getOpenFileName(this, "选择参考影像", "", "Images (*.tif *.tiff *.png *.jpg)");
    if (!f.isEmpty()) m_referEdit->setText(f);
}
void ReconstructPage::onBrowseMask() {
    QString f = QFileDialog::getOpenFileName(this, "选择云掩膜", "", "Images (*.tif *.tiff *.png *.jpg)");
    if (!f.isEmpty()) m_maskEdit->setText(f);
}

void ReconstructPage::execute(const QString &userSavePath) {
    QString algoType = m_algoSelectCombo->currentData().toString();
    emit logMessage(QString(">> [Reconstruct] 开始执行 [%1]...").arg(m_algoSelectCombo->currentText()));

    // 校验输入
    if(m_targetEdit->text().isEmpty() || m_referEdit->text().isEmpty() || m_maskEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入不完整", "请补全 Target, Reference 和 Mask 三个文件路径。");
        return;
    }

    try {
        // 1. 读取数据
        emit logMessage(">> 正在加载影像数据...");
        auto targetImg = RSPIP::IO::GeoImageRead(m_targetEdit->text().toStdString());
        auto referImg = RSPIP::IO::GeoImageRead(m_referEdit->text().toStdString());
        auto maskImg = RSPIP::IO::CloudMaskImageRead(m_maskEdit->text().toStdString());

        if (!targetImg || !referImg || !maskImg) {
            emit logMessage("错误: 影像或掩膜文件读取失败。");
            return;
        }

        // 2. 准备算法
        std::unique_ptr<RSPIP::Algorithm::ReconstructAlgorithm::ReconstructAlgorithmBase> algorithm;

        if (algoType == "Isophote") {
            algorithm = std::make_unique<RSPIP::Algorithm::ReconstructAlgorithm::IsophoteConstrain>(*targetImg, *referImg, *maskImg);
        }
        else if (algoType == "ColorBalance") {
            algorithm = std::make_unique<RSPIP::Algorithm::ReconstructAlgorithm::ColorBalanceReconstruct>(*targetImg, *referImg, *maskImg);
        }
        else if (algoType == "Simple") {
            algorithm = std::make_unique<RSPIP::Algorithm::ReconstructAlgorithm::Simple>(*targetImg, *referImg, *maskImg);
        }

        // 3. 执行
        if (algorithm) {
            emit logMessage(">> 算法计算中 (可能需要较长时间)...");
            // 这里在主线程执行可能会卡顿UI，实际项目中建议放入 QThread
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

    } catch (const std::exception& e) {
        emit logMessage(QString("异常: %1").arg(e.what()));
    }
}
