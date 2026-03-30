#include "PixelThresholdPanel.h"

#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QStandardPaths>
#include <QVBoxLayout>

#include "Algorithm/CloudDetection/PixelThreshold.h"
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"

namespace Panels::CloudDetection {

PixelThresholdPanel::PixelThresholdPanel(QWidget *parent) : AlgorithmPanelBase(parent) {
    _SetupUi();
}

QString PixelThresholdPanel::AlgorithmDescription() const {
    return QString("【%1】\n\n"
                   "功能: 对输入影像列表批量执行云检测，输出二值云掩膜。\n"
                   "方法: 使用像素阈值法，可基于灰度或蓝色波段进行阈值检测。\n"
                   "参数: 检测通道(灰度/蓝色波段)、阈值范围 0-255，默认 240。")
        .arg(AlgorithmName());
}

void PixelThresholdPanel::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";
    _InputSelector = new FileListWidget("输入影像列表 (Inputs):", imgFilter, this);
    layout->addWidget(_InputSelector);

    layout->addWidget(new QLabel("阈值检测通道:", this));
    _ModeCombo = new QComboBox(this);
    _ModeCombo->addItem("灰度", static_cast<int>(RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::Gray));
    _ModeCombo->addItem("蓝色波段", static_cast<int>(RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::BlueBandOnly));
    layout->addWidget(_ModeCombo);

    _ThresholdSpin = new QSpinBox(this);
    _ThresholdSpin->setRange(0, 255);
    _ThresholdSpin->setValue(240);
    _ThresholdSpin->setSingleStep(1);
    _ThresholdSpin->setPrefix("云阈值: ");
    layout->addWidget(_ThresholdSpin);

    layout->addStretch();
}

bool PixelThresholdPanel::ValidateInput() {
    if (_InputSelector->Files().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请至少选择一张输入影像");
        return false;
    }
    return true;
}

std::function<bool()> PixelThresholdPanel::BuildTask(const QString &globalSavePath) {
    const QStringList inputFiles = _InputSelector->Files();
    const auto mode = static_cast<RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode>(
        _ModeCombo->currentData().toInt());
    const QString modeText = _ModeCombo->currentText();
    const unsigned char threshold = static_cast<unsigned char>(_ThresholdSpin->value());

    return [this, inputFiles, mode, modeText, threshold, globalSavePath]() {
        PostLog(QString(">> [CloudDetection::PixelThreshold] 待处理影像数量: %1，检测通道: %2")
                    .arg(inputFiles.count())
                    .arg(modeText));

        try {
            QString outputDir;
            QString outputPrefix;
            bool treatAsDirectory = false;

            if (globalSavePath.isEmpty()) {
                outputDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
                outputPrefix = "cloud_mask";
                treatAsDirectory = true;
                PostLog(">> 使用临时目录保存结果: " + outputDir);
            } else {
                QFileInfo outputInfo(globalSavePath);
                treatAsDirectory = outputInfo.isDir() ||
                                   (!outputInfo.exists() && outputInfo.suffix().isEmpty());
                if (treatAsDirectory) {
                    outputDir = QDir::cleanPath(globalSavePath);
                    outputPrefix = "cloud_mask";
                } else {
                    outputDir = outputInfo.absolutePath();
                    outputPrefix = outputInfo.completeBaseName().isEmpty()
                                       ? "cloud_mask"
                                       : outputInfo.completeBaseName();
                }
                PostLog(">> 使用指定输出目录: " + outputDir);
            }

            if (outputDir.isEmpty()) {
                PostLog("错误: 输出目录无效。");
                return false;
            }
            if (!QDir().mkpath(outputDir)) {
                PostLog("错误: 无法创建输出目录: " + outputDir);
                return false;
            }

            const bool singleExactOutput = (inputFiles.count() == 1 &&
                                            !globalSavePath.isEmpty() &&
                                            !treatAsDirectory);

            int successCount = 0;
            for (int i = 0; i < inputFiles.count(); ++i) {
                const QString &inputPath = inputFiles.at(i);
                PostLog(QString(">> [%1/%2] 读取: %3").arg(i + 1).arg(inputFiles.count()).arg(inputPath));

                auto inputImage = RSPIP::IO::GeoImageRead(inputPath.toStdString());
                if (!inputImage) {
                    PostLog("错误: 无法读取输入影像，已跳过。");
                    continue;
                }

                RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThreshold algorithm(*inputImage);
                algorithm.SetThreshold(threshold);
                algorithm.SetThresholdMode(mode);

                if (mode == RSPIP::Algorithm::CloudDetectionAlgorithm::PixelThresholdMode::BlueBandOnly &&
                    inputImage->GetBandCounts() < 3) {
                    PostLog("错误: 当前影像波段数不足 3，无法使用蓝色波段阈值检测，已跳过。");
                    continue;
                }

                PostLog(QString(">> [%1/%2] 正在执行云检测，模式=%3，阈值=%4 ...")
                            .arg(i + 1)
                            .arg(inputFiles.count())
                            .arg(modeText)
                            .arg(static_cast<int>(threshold)));
                algorithm.Execute();

                QString finalSavePath;
                if (singleExactOutput) {
                    finalSavePath = globalSavePath;
                } else {
                    QFileInfo inputInfo(inputPath);
                    finalSavePath = QString("%1/%2_%3.tif")
                                        .arg(outputDir)
                                        .arg(outputPrefix)
                                        .arg(inputInfo.completeBaseName());
                }

                bool saved = RSPIP::IO::SaveImage(algorithm.AlgorithmResult,
                                                  QFileInfo(finalSavePath).absolutePath().toStdString(),
                                                  QFileInfo(finalSavePath).fileName().toStdString());
                if (!saved) {
                    PostLog("错误: 云掩膜保存失败: " + finalSavePath);
                    continue;
                }

                PostLog(">> 保存成功: " + finalSavePath);
                ++successCount;
            }

            PostLog(QString(">> 云检测完成，成功 %1 / %2").arg(successCount).arg(inputFiles.count()));
            return successCount > 0;
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::CloudDetection
