#include "MatchStatisticsPanel.h"
#include "Common/FileSelectWidget.h"

#include <QDateTime>
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

    _TargetSelect = new FileSelectWidget("目标影像 (Target):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_TargetSelect);

    _ReferSelect = new FileSelectWidget("参考影像 (Reference):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    layout->addWidget(_ReferSelect);

    _MaskSelect = new FileSelectWidget("掩膜文件 (Mask, 可选):", imgFilter, FileSelectWidget::Mode::FileOpen, this);
    _MaskSelect->SetPlaceholderText("留空则不使用掩膜...");
    layout->addWidget(_MaskSelect);

    layout->addStretch();
}

bool MatchStatisticsPanel::ValidateInput() {
    if (_TargetSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择目标影像");
        return false;
    }
    if (_ReferSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择参考影像");
        return false;
    }
    return true;
}

std::function<bool()> MatchStatisticsPanel::BuildTask(const QString &globalSavePath) {
    const std::string targetPath = _TargetSelect->CurrentPath().toStdString();
    const std::string referPath = _ReferSelect->CurrentPath().toStdString();
    const std::string maskPath = _MaskSelect->CurrentPath().toStdString();

    return [this, targetPath, referPath, maskPath, globalSavePath]() {
        PostLog(">> [MatchStatistics] 正在读取影像数据...");

        try {
            auto targetImage = RSPIP::IO::GeoImageRead(targetPath);
            auto referImage = RSPIP::IO::GeoImageRead(referPath);

            if (!targetImage || !referImage) {
                PostLog("错误: 无法读取影像数据。");
                return false;
            }

            std::unique_ptr<RSPIP::CloudMask> maskImage;
            if (!maskPath.empty()) {
                maskImage = RSPIP::IO::CloudMaskImageRead(maskPath);
                if (!maskImage) {
                    PostLog("错误: 无法读取掩膜文件。");
                    return false;
                }
            } else {
                maskImage = std::make_unique<RSPIP::CloudMask>();
            }

            RSPIP::Algorithm::ColorBalanceAlgorithm::MatchStatistics algorithm(
                *targetImage, *referImage, *maskImage);

            PostLog(">> 算法正在执行...");
            algorithm.Execute();

            QString finalSavePath = globalSavePath;
            if (finalSavePath.isEmpty()) {
                QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
                finalSavePath = QString("%1/match_stat_%2.tif")
                                    .arg(tempDir)
                                    .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
                PostLog(">> 使用自动生成的路径: " + finalSavePath);
            }

            bool saved = RSPIP::IO::SaveImage(algorithm.AlgorithmResult,
                                              QFileInfo(finalSavePath).absolutePath().toStdString(),
                                              QFileInfo(finalSavePath).fileName().toStdString());

            if (saved) {
                PostLog(">> 处理成功！");
                return true;
            }

            PostLog("错误: 保存结果失败。");
            return false;

        } catch (const std::exception &e) {
            PostLog(QString("异常: %1").arg(e.what()));
            return false;
        }
    };
}

} // namespace Panels::ColorBalance



