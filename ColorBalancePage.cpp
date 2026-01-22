#include "ColorBalancePage.h"

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
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QDateTime>

// --- 引入 RSPIP 库头文件 ---
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"
#include "Basic/GeoImage.h"
#include "Basic/Image.h" // 确保包含 Image 定义

// 引入具体的算法头文件
#include "Algorithm/ColorBalance/MatchStatistic.h"

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

    auto createRow = [this](const QString &title, QLineEdit *&edit, auto slot, const QString &placeholder = "") {
        QLabel *lbl = new QLabel(title, this);
        edit = new QLineEdit(this);
        edit->setPlaceholderText(placeholder);
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

    formLayout->addLayout(createRow("目标影像 (Target):", m_targetEdit, &ColorBalancePage::onBrowseTarget));
    formLayout->addLayout(createRow("参考影像 (Reference):", m_referEdit, &ColorBalancePage::onBrowseRefer));
    formLayout->addLayout(createRow("掩膜文件 (Mask, 可选):", m_maskEdit, &ColorBalancePage::onBrowseMask, "留空则不使用掩膜..."));

    layout->addWidget(inputGroup);
    layout->addStretch();
}

void ColorBalancePage::onBrowseTarget() {
    QString f = QFileDialog::getOpenFileName(this, "选择目标影像", "", "Images (*.tif *.tiff *.png *.jpg *.bmp)");
    if (!f.isEmpty()) m_targetEdit->setText(f);
}

void ColorBalancePage::onBrowseRefer() {
    QString f = QFileDialog::getOpenFileName(this, "选择参考影像", "", "Images (*.tif *.tiff *.png *.jpg *.bmp)");
    if (!f.isEmpty()) m_referEdit->setText(f);
}

void ColorBalancePage::onBrowseMask() {
    QString f = QFileDialog::getOpenFileName(this, "选择掩膜文件", "", "Images (*.tif *.tiff *.png *.jpg *.bmp)");
    if (!f.isEmpty()) m_maskEdit->setText(f);
}

void ColorBalancePage::execute(const QString &userSavePath) {
    QString targetPath = m_targetEdit->text();
    QString referPath = m_referEdit->text();
    QString maskPath = m_maskEdit->text();
    QString algoType = m_algoSelectCombo->currentData().toString();

    // 1. 校验必填项
    if(targetPath.isEmpty() || referPath.isEmpty()) {
        QMessageBox::warning(this, "输入不完整", "请先选择目标影像和参考影像。");
        return;
    }

    emit logMessage(QString(">> [ColorBalance] 开始执行 [%1]...").arg(m_algoSelectCombo->currentText()));

    try {
        // 2. 读取主数据
        auto targetImg = RSPIP::IO::GeoImageRead(targetPath.toStdString());
        auto referImg = RSPIP::IO::GeoImageRead(referPath.toStdString());
        if (!targetImg || !referImg) {
            emit logMessage("错误: 目标或参考影像读取失败。");
            return;
        }

        // 3. 读取掩膜 (如果有)
        // 构造一个默认的空 Image 对象作为 fallback
        RSPIP::Image defaultMask;
        const RSPIP::Image* maskPtr = &defaultMask;
        std::unique_ptr<RSPIP::Image> loadedMask;

        if (!maskPath.isEmpty()) {
            emit logMessage(QString(">> 正在加载掩膜: %1").arg(QFileInfo(maskPath).fileName()));
            // 尝试作为 GeoImage 读取，如果失败可能是普通图片
            auto geoMask = RSPIP::IO::GeoImageRead(maskPath.toStdString());
            if (geoMask) {
                loadedMask = std::move(geoMask);
            } else {
                // 尝试作为普通图片读取
                loadedMask = RSPIP::IO::NormalImageRead(maskPath.toStdString());
            }

            if (loadedMask) {
                maskPtr = loadedMask.get();
            } else {
                emit logMessage("警告: 掩膜文件读取失败，将忽略掩膜继续处理。");
            }
        }

        // 4. 执行算法
        RSPIP::Image resultImage;
        bool success = false;

        if (algoType == "MatchStatistics") {
            // 传入 maskPtr (如果不为空则是加载的掩膜，否则是默认空对象)
            RSPIP::Algorithm::ColorBalanceAlgorithm::MatchStatistics algo(*targetImg, *referImg, *maskPtr);
            algo.Execute();
            resultImage = std::move(algo.AlgorithmResult);
            success = true;
        }
        else {
            emit logMessage("错误: 未知的算法类型。");
            return;
        }

        if (!success || resultImage.ImageData.empty()) {
            emit logMessage("错误: 算法结果为空。");
            return;
        }

        // 5. 确定输出路径
        QString finalSavePath = userSavePath;
        if (finalSavePath.isEmpty()) {
            QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            QString ext = QFileInfo(targetPath).suffix();
            if(ext.isEmpty()) ext = "tif";

            finalSavePath = QString("%1/rspip_cb_%2.%3")
                                .arg(tempDir)
                                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
                                .arg(ext);
            emit logMessage(QString(">> 使用临时保存路径: %1").arg(finalSavePath));
        }

        // 6. 保存与显示
        QFileInfo saveInfo(finalSavePath);
        bool saved = RSPIP::IO::SaveImage(resultImage,
                                          saveInfo.absolutePath().toStdString(),
                                          saveInfo.fileName().toStdString());

        if (saved) {
            emit logMessage(">> 处理成功并保存！");
            emit resultFileReady(finalSavePath);
        } else {
            emit logMessage("错误: 结果文件保存失败。");
        }

    } catch (const std::exception& e) {
        emit logMessage(QString("异常: %1").arg(e.what()));
    }
}
