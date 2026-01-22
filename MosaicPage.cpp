#include "MosaicPage.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QFileInfo>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QDateTime>

// --- RSPIP Headers ---
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"
#include "Algorithm/Mosaic/Simple.h"
#include "Algorithm/Mosaic/ShowOverLap.h"
#include "Algorithm/Mosaic/DynamicPatch.h"

MosaicPage::MosaicPage(QWidget *parent) : IAlgorithmPage(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 1. 算法选择
    QGroupBox *algoGroup = new QGroupBox("1. 算法选择", this);
    QVBoxLayout *algoLayout = new QVBoxLayout(algoGroup);
    m_algoSelectCombo = new QComboBox(this);
    // Data 字段存储类标识符
    m_algoSelectCombo->addItem("Simple (直接覆盖)", "Simple");
    m_algoSelectCombo->addItem("ShowOverLap (显示重叠区域)", "ShowOverLap");
    m_algoSelectCombo->addItem("DynamicPatch (动态补丁去云)", "DynamicPatch");

    // 连接信号处理 UI 显隐
    connect(m_algoSelectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MosaicPage::onAlgoChanged);

    algoLayout->addWidget(m_algoSelectCombo);
    layout->addWidget(algoGroup);

    // 2. 输入影像列表
    QGroupBox *inputGroup = new QGroupBox("2. 输入影像 (Images)", this);
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    m_imageList = new QListWidget(this);
    QPushButton *addBtn = new QPushButton("添加影像...", this);
    connect(addBtn, &QPushButton::clicked, this, &MosaicPage::onAddImages);

    inputLayout->addWidget(m_imageList);
    inputLayout->addWidget(addBtn);
    layout->addWidget(inputGroup);

    // 3. 输入掩膜列表 (默认隐藏，仅 DynamicPatch 需要)
    m_maskGroup = new QGroupBox("3. 输入云掩膜 (Cloud Masks)", this);
    QVBoxLayout *maskLayout = new QVBoxLayout(m_maskGroup);
    m_maskList = new QListWidget(this);
    QPushButton *addMaskBtn = new QPushButton("添加掩膜...", this);
    connect(addMaskBtn, &QPushButton::clicked, this, &MosaicPage::onAddMasks);

    maskLayout->addWidget(m_maskList);
    maskLayout->addWidget(addMaskBtn);
    layout->addWidget(m_maskGroup);

    // 初始化 UI 状态
    onAlgoChanged(0);

    layout->addStretch();
}

void MosaicPage::onAlgoChanged(int index) {
    QString algoData = m_algoSelectCombo->itemData(index).toString();
    // 只有 DynamicPatch 需要显示掩膜输入框
    m_maskGroup->setVisible(algoData == "DynamicPatch");
}

void MosaicPage::onAddImages() {
    QStringList files = QFileDialog::getOpenFileNames(this, "选择影像", "", "Images (*.tif *.tiff *.png *.jpg)");
    if (!files.isEmpty()) {
        m_imageList->addItems(files);
        emit logMessage(QString("已添加 %1 张影像").arg(files.size()));
    }
}

void MosaicPage::onAddMasks() {
    QStringList files = QFileDialog::getOpenFileNames(this, "选择掩膜", "", "Images (*.tif *.tiff *.png *.jpg)");
    if (!files.isEmpty()) {
        m_maskList->addItems(files);
        emit logMessage(QString("已添加 %1 张掩膜").arg(files.size()));
    }
}

void MosaicPage::execute(const QString &userSavePath) {
    QString algoType = m_algoSelectCombo->currentData().toString();
    emit logMessage(QString(">> [Mosaic] 开始执行 [%1]...").arg(m_algoSelectCombo->currentText()));

    if (m_imageList->count() < 2) {
        QMessageBox::warning(this, "输入不足", "镶嵌至少需要两张影像。");
        return;
    }

    try {
        // 1. 读取所有 GeoImage
        std::vector<RSPIP::GeoImage> images;
        images.reserve(m_imageList->count());

        emit logMessage(">> 正在加载影像数据...");
        for(int i=0; i<m_imageList->count(); ++i) {
            QString path = m_imageList->item(i)->text();
            auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
            if(imgPtr) {
                images.push_back(std::move(*imgPtr));
            } else {
                emit logMessage("错误: 无法读取影像 " + path);
                return;
            }
        }

        // 2. 准备算法对象
        std::unique_ptr<RSPIP::Algorithm::MosaicAlgorithm::MosaicAlgorithmBase> algorithm;

        if (algoType == "Simple") {
            algorithm = std::make_unique<RSPIP::Algorithm::MosaicAlgorithm::Simple>(images);
        }
        else if (algoType == "ShowOverLap") {
            algorithm = std::make_unique<RSPIP::Algorithm::MosaicAlgorithm::ShowOverLap>(images);
        }
        else if (algoType == "DynamicPatch") {
            // DynamicPatch 需要读取 CloudMasks
            if (m_maskList->count() != m_imageList->count()) {
                QMessageBox::warning(this, "数量不匹配", "DynamicPatch 算法要求掩膜数量必须与影像数量一致。");
                return;
            }

            std::vector<RSPIP::CloudMask> masks;
            masks.reserve(m_maskList->count());
            emit logMessage(">> 正在加载掩膜数据...");

            for(int i=0; i<m_maskList->count(); ++i) {
                QString path = m_maskList->item(i)->text();
                auto maskPtr = RSPIP::IO::CloudMaskImageRead(path.toStdString());
                if(maskPtr) {
                    masks.push_back(std::move(*maskPtr));
                } else {
                    emit logMessage("错误: 无法读取掩膜 " + path);
                    return;
                }
            }

            algorithm = std::make_unique<RSPIP::Algorithm::MosaicAlgorithm::DynamicPatch>(images, masks);
        }

        // 3. 执行
        if (algorithm) {
            emit logMessage(">> 算法计算中，请稍候...");
            algorithm->Execute();

            // 4. 处理结果路径
            QString finalSavePath = userSavePath;
            if (finalSavePath.isEmpty()) {
                QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
                finalSavePath = QString("%1/mosaic_result_%2.tif")
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
                emit logMessage(">> 镶嵌完成并保存！");
                emit resultFileReady(finalSavePath);
            } else {
                emit logMessage("错误: 结果保存失败。");
            }
        }

    } catch (const std::exception& e) {
        emit logMessage(QString("异常: %1").arg(e.what()));
    }
}
