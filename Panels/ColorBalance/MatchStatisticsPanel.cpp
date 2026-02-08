#include "MatchStatisticsPanel.h"
#include "Common/FileSelectWidget.h"

#include <QApplication>
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

bool MatchStatisticsPanel::ValidateInput() const {
    if (_TargetSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(const_cast<MatchStatisticsPanel *>(this), "输入错误", "请选择目标影像");
        return false;
    }
    if (_ReferSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(const_cast<MatchStatisticsPanel *>(this), "输入错误", "请选择参考影像");
        return false;
    }
    return true;
}

bool MatchStatisticsPanel::Run(const QString &globalSavePath) {
    std::string targetPath = _TargetSelect->CurrentPath().toStdString();
    std::string referPath = _ReferSelect->CurrentPath().toStdString();
    std::string maskPath = _MaskSelect->CurrentPath().toStdString();

    emit LogMessage(">> [MatchStatistics] 正在读取影像数据...");
    QApplication::processEvents(); // 刷新 UI 防止卡死

    try {
        auto targetImage = RSPIP::IO::GeoImageRead(targetPath);
        auto referImage = RSPIP::IO::GeoImageRead(referPath);

        if (!targetImage || !referImage) {
            emit LogMessage("错误: 无法读取影像数据。");
            return false;
        }

        std::unique_ptr<RSPIP::CloudMask> maskImage;
        if (!maskPath.empty()) {
            maskImage = RSPIP::IO::CloudMaskImageRead(maskPath);
        } else {
            // 如果没有掩膜，创建一个空掩膜对象
            maskImage = std::make_unique<RSPIP::CloudMask>();
        }

        RSPIP::Algorithm::ColorBalanceAlgorithm::MatchStatistics algorithm(
            *targetImage, *referImage, *maskImage);

        emit LogMessage(">> 算法正在执行...");
        QApplication::processEvents();

        algorithm.Execute();

        QString finalSavePath = globalSavePath;
        if (finalSavePath.isEmpty()) {
            QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            finalSavePath = QString("%1/match_stat_%2.tif")
                                .arg(tempDir)
                                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
            emit LogMessage(">> 使用自动生成的路径: " + finalSavePath);
        }

        bool saved = RSPIP::IO::SaveImage(algorithm.AlgorithmResult,
                                          QFileInfo(finalSavePath).absolutePath().toStdString(),
                                          QFileInfo(finalSavePath).fileName().toStdString());

        if (saved) {
            emit LogMessage(">> 处理成功！");
            return true;
        } else {
            emit LogMessage("错误: 保存结果失败。");
            return false;
        }

    } catch (const std::exception &e) {
        emit LogMessage(QString("异常: %1").arg(e.what()));
        return false;
    }
}

} // namespace Panels::ColorBalance