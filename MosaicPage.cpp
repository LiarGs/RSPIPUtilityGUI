#include "MosaicPage.h"
#include "Common/FileListWidget.h" // 引入通用文件列表控件

#include <QComboBox>
#include <QDateTime>
#include <QFileInfo>
#include <QGroupBox>
#include <QMessageBox>
#include <QStandardPaths>
#include <QVBoxLayout>

// --- RSPIP Headers ---
#include "Algorithm/Mosaic/DynamicPatch.h"
#include "Algorithm/Mosaic/ShowOverLap.h"
#include "Algorithm/Mosaic/Simple.h"
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"

MosaicPage::MosaicPage(QWidget *parent) : IAlgorithmPage(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 1. 算法选择
    QGroupBox *algoGroup = new QGroupBox("1. 算法选择", this);
    QVBoxLayout *algoLayout = new QVBoxLayout(algoGroup);
    m_algoSelectCombo = new QComboBox(this);
    m_algoSelectCombo->addItem("Simple (直接覆盖)", "Simple");
    m_algoSelectCombo->addItem("ShowOverLap (显示重叠区域)", "ShowOverLap");
    m_algoSelectCombo->addItem("DynamicPatch (动态补丁去云)", "DynamicPatch");

    connect(m_algoSelectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MosaicPage::onAlgoChanged);

    algoLayout->addWidget(m_algoSelectCombo);
    layout->addWidget(algoGroup);

    // 2. 输入影像 (使用新控件)
    // 移除了手动创建 ListWidget 和 Button 的代码
    m_imageSelector = new FileListWidget("2. 输入影像列表 (Images)",
                                         "Images (*.tif *.tiff *.png *.jpg)",
                                         this);
    layout->addWidget(m_imageSelector);

    // 3. 输入掩膜 (使用新控件)
    m_maskGroup = new QGroupBox("3. 输入云掩膜 (Cloud Masks)", this);
    QVBoxLayout *maskLayout = new QVBoxLayout(m_maskGroup);

    m_maskSelector = new FileListWidget("掩膜列表 (需与影像一一对应)",
                                        "Images (*.tif *.tiff *.png *.jpg)",
                                        this);
    maskLayout->addWidget(m_maskSelector);
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

void MosaicPage::execute(const QString &userSavePath) {
    QString algoType = m_algoSelectCombo->currentData().toString();

    // 从控件获取文件列表
    QStringList imageFiles = m_imageSelector->files();

    emit logMessage(QString(">> [Mosaic] 开始执行 [%1]...").arg(m_algoSelectCombo->currentText()));

    if (imageFiles.count() < 2) {
        QMessageBox::warning(this, "输入不足", "镶嵌至少需要两张影像。");
        return;
    }

    try {
        // 1. 读取所有 GeoImage
        std::vector<RSPIP::GeoImage> images;
        images.reserve(imageFiles.count());

        emit logMessage(">> 正在加载影像数据...");
        for (const QString &path : imageFiles) {
            auto imgPtr = RSPIP::IO::GeoImageRead(path.toStdString());
            if (imgPtr) {
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
        } else if (algoType == "ShowOverLap") {
            algorithm = std::make_unique<RSPIP::Algorithm::MosaicAlgorithm::ShowOverLap>(images);
        } else if (algoType == "DynamicPatch") {
            // 获取掩膜列表
            QStringList maskFiles = m_maskSelector->files();

            if (maskFiles.count() != imageFiles.count()) {
                QMessageBox::warning(this, "数量不匹配", "DynamicPatch 算法要求掩膜数量必须与影像数量一致。");
                return;
            }

            std::vector<RSPIP::CloudMask> masks;
            masks.reserve(maskFiles.count());
            emit logMessage(">> 正在加载掩膜数据...");

            for (const QString &path : maskFiles) {
                auto maskPtr = RSPIP::IO::CloudMaskImageRead(path.toStdString());
                if (maskPtr) {
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

    } catch (const std::exception &e) {
        emit logMessage(QString("异常: %1").arg(e.what()));
    }
}