#include "MatchStatisticsPanel.h"
#include "Common/FileListWidget.h"
#include "Common/FileSelectWidget.h"

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QVBoxLayout>

// --- 引入 RSPIP 后端库 ---
#include "Algorithm/ColorBalance/MatchStatistic.h"
#include "Basic/CloudMask.h"
#include "Basic/GeoImage.h"
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"

namespace Panels::ColorBalance {

MatchStatisticsPanel::MatchStatisticsPanel(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void MatchStatisticsPanel::_SetupUi() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.bmp)";

    _ReferSelect = new FileSelectWidget("基准影像 (Reference):",
                                        imgFilter,
                                        FileSelectWidget::Mode::FileOpen,
                                        this);
    layout->addWidget(_ReferSelect);

    _InputSelector = new FileListWidget("待匀色影像列表 (Targets):", imgFilter, this);
    layout->addWidget(_InputSelector);

    _MaskSelector = new FileListWidget("掩膜列表 (Mask，可选，需与待匀色影像一一对应):",
                                       imgFilter,
                                       this);
    layout->addWidget(_MaskSelector);

    layout->addStretch();
}

bool MatchStatisticsPanel::ValidateInput() {
    if (_ReferSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择基准影像");
        return false;
    }
    if (_InputSelector->Files().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请至少选择一张待匀色影像");
        return false;
    }
    if (!_MaskSelector->Files().isEmpty() &&
        _MaskSelector->Files().count() != _InputSelector->Files().count()) {
        QMessageBox::warning(this, "数量不匹配",
                             "如果提供掩膜列表，其数量必须与待匀色影像数量完全一致。");
        return false;
    }
    return true;
}

std::function<bool()> MatchStatisticsPanel::BuildTask(const QString &globalSavePath) {
    const QString referPath = _ReferSelect->CurrentPath();
    const QStringList inputFiles = _InputSelector->Files();
    const QStringList maskFiles = _MaskSelector->Files();

    return [this, referPath, inputFiles, maskFiles, globalSavePath]() {
        PostLog(QString(">> [MatchStatistics] 待处理影像数量: %1").arg(inputFiles.count()));

        try {
            PostLog(">> 正在读取基准影像...");
            auto referImage = RSPIP::IO::GeoImageRead(referPath.toStdString());
            if (!referImage) {
                PostLog("错误: 无法读取基准影像。");
                return false;
            }

            QString outputDir;
            QString outputPrefix;
            bool treatAsDirectory = false;
            if (globalSavePath.isEmpty()) {
                outputDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
                outputPrefix = "match_stat";
                treatAsDirectory = true;
                PostLog(">> 使用临时目录保存结果: " + outputDir);
            } else {
                QFileInfo outputInfo(globalSavePath);
                treatAsDirectory = outputInfo.isDir() ||
                                   (!outputInfo.exists() && outputInfo.suffix().isEmpty());
                if (treatAsDirectory) {
                    outputDir = QDir::cleanPath(globalSavePath);
                    outputPrefix = "match_stat";
                } else {
                    outputDir = outputInfo.absolutePath();
                    outputPrefix = outputInfo.completeBaseName().isEmpty()
                                       ? "match_stat"
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
                const QString maskPath = (i < maskFiles.count()) ? maskFiles.at(i) : QString();

                try {
                    PostLog(QString(">> [%1/%2] 正在读取待匀色影像: %3")
                                .arg(i + 1)
                                .arg(inputFiles.count())
                                .arg(inputPath));
                    auto targetImage = RSPIP::IO::GeoImageRead(inputPath.toStdString());
                    if (!targetImage) {
                        PostLog("错误: 无法读取待匀色影像，已跳过。");
                        continue;
                    }

                    std::unique_ptr<RSPIP::CloudMask> maskImage;
                    if (!maskPath.isEmpty()) {
                        PostLog(QString(">> [%1/%2] 正在读取掩膜: %3")
                                    .arg(i + 1)
                                    .arg(inputFiles.count())
                                    .arg(maskPath));
                        maskImage = RSPIP::IO::CloudMaskImageRead(maskPath.toStdString());
                        if (!maskImage) {
                            PostLog("错误: 无法读取掩膜文件，已跳过当前影像。");
                            continue;
                        }
                    } else {
                        maskImage = std::make_unique<RSPIP::CloudMask>();
                    }

                    RSPIP::Algorithm::ColorBalanceAlgorithm::MatchStatistics algorithm(
                        *targetImage, *referImage, *maskImage);
                    PostLog(QString(">> [%1/%2] 正在执行匀色...")
                                .arg(i + 1)
                                .arg(inputFiles.count()));
                    algorithm.Execute();

                    QString finalSavePath;
                    if (singleExactOutput) {
                        finalSavePath = globalSavePath;
                    } else {
                        QFileInfo inputInfo(inputPath);
                        finalSavePath = QString("%1/%2_%3_%4.tif")
                                            .arg(outputDir)
                                            .arg(outputPrefix)
                                            .arg(i + 1)
                                            .arg(inputInfo.completeBaseName());
                    }

                    const bool saved = RSPIP::IO::SaveImage(
                        algorithm.AlgorithmResult,
                        QFileInfo(finalSavePath).absolutePath().toStdString(),
                        QFileInfo(finalSavePath).fileName().toStdString());
                    if (!saved) {
                        PostLog("错误: 保存结果失败: " + finalSavePath);
                        continue;
                    }

                    PostLog(">> 保存成功: " + finalSavePath);
                    ++successCount;
                } catch (const std::exception &e) {
                    PostLog(QString("异常: [%1/%2] %3")
                                .arg(i + 1)
                                .arg(inputFiles.count())
                                .arg(e.what()));
                }
            }

            PostLog(QString(">> MatchStatistics 批处理完成，成功 %1 / %2")
                        .arg(successCount)
                        .arg(inputFiles.count()));
            return successCount > 0;
        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::ColorBalance
