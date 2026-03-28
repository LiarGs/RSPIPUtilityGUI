#include "EvaluatorPanelBase.h"

#include "Basic/Image.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>
#include <QStandardPaths>
#include <QTextStream>
#include <QVBoxLayout>

namespace Panels::Evaluation {

EvaluatorPanelBase::EvaluatorPanelBase(QWidget *parent)
    : AlgorithmPanelBase(parent) {
    _SetupUi();
}

void EvaluatorPanelBase::_SetupUi() {
    _MainLayout = new QVBoxLayout(this);
    _MainLayout->setContentsMargins(0, 0, 0, 0);
    _MainLayout->setSpacing(10);

    const QString imgFilter = "Images (*.tif *.tiff *.png *.jpg *.jpeg *.bmp)";

    _ImageSelect = new FileSelectWidget("待评估影像 (Image):",
                                        imgFilter,
                                        FileSelectWidget::Mode::FileOpen,
                                        this);
    _MainLayout->addWidget(_ImageSelect);

    _ReferenceSelect = new FileSelectWidget("参考影像 (Reference):",
                                            imgFilter,
                                            FileSelectWidget::Mode::FileOpen,
                                            this);
    _MainLayout->addWidget(_ReferenceSelect);

    _MaskSelect = new FileSelectWidget("掩膜文件 (Mask, 可选):",
                                       imgFilter,
                                       FileSelectWidget::Mode::FileOpen,
                                       this);
    _MaskSelect->SetPlaceholderText("留空则对整幅影像计算...");
    _MainLayout->addWidget(_MaskSelect);

    _BoundaryOnlyCheck = new QCheckBox("仅计算 Mask 边界像素", this);
    _BoundaryOnlyCheck->setVisible(false);
    _BoundaryOnlyCheck->setEnabled(false);
    _MainLayout->addWidget(_BoundaryOnlyCheck);

    connect(_MaskSelect, &FileSelectWidget::PathChanged, this,
            [this](const QString &) { _UpdateBoundaryOnlyState(); });

    _MainLayout->addStretch();
}

bool EvaluatorPanelBase::ValidateInput() {
    if (_ImageSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择待评估影像");
        return false;
    }

    if (_ReferenceSelect->CurrentPath().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择参考影像");
        return false;
    }

    if (_IsBoundaryOnlyRequested() && !_HasMaskInput()) {
        QMessageBox::warning(this, "输入错误", "仅在提供掩膜文件后才能勾选仅计算边界像素");
        return false;
    }

    return true;
}

bool EvaluatorPanelBase::_ValidatePairwiseCompatible(const RSPIP::Image &imageData,
                                                     const RSPIP::Image &referenceImage) {
    if (imageData.Width() != referenceImage.Width() ||
        imageData.Height() != referenceImage.Height()) {
        PostLog("错误: 两幅影像的宽高不一致，无法进行指标评估。");
        return false;
    }

    return true;
}

bool EvaluatorPanelBase::_ValidateMaskCompatible(const RSPIP::Image &imageData,
                                                 const RSPIP::Image &maskImage) {
    if (imageData.Width() != maskImage.Width() ||
        imageData.Height() != maskImage.Height()) {
        PostLog("错误: 掩膜与待评估影像的宽高不一致，无法进行指标评估。");
        return false;
    }

    return true;
}

bool EvaluatorPanelBase::_SaveTextResult(const QString &content,
                                         const QString &userPath,
                                         const QString &prefix) {
    QString finalSavePath = userPath.trimmed();
    if (finalSavePath.isEmpty()) {
        const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        finalSavePath = QString("%1/%2_%3.txt")
                            .arg(tempDir, prefix, QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
        PostLog(">> 使用自动生成的路径: " + finalSavePath);
    } else {
        const QFileInfo userInfo(finalSavePath);
        const QString suffix = userInfo.suffix().toLower();
        const QStringList imageSuffixes = {"tif", "tiff", "png", "jpg", "jpeg", "bmp"};

        if (suffix.isEmpty() || imageSuffixes.contains(suffix)) {
            const QString baseName = userInfo.completeBaseName().isEmpty()
                                         ? prefix
                                         : userInfo.completeBaseName();
            finalSavePath = userInfo.dir().filePath(baseName + ".txt");
            PostLog(">> 评估结果将保存为文本文件: " + finalSavePath);
        }
    }

    const QFileInfo finalInfo(finalSavePath);
    const QString parentDir = finalInfo.absolutePath();
    if (!parentDir.isEmpty() && !QDir().mkpath(parentDir)) {
        PostLog("错误: 无法创建结果目录。");
        return false;
    }

    QFile outputFile(finalSavePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        PostLog("错误: 无法写入评估结果文件。");
        return false;
    }

    QTextStream stream(&outputFile);
    stream << content;
    outputFile.close();

    PostLog(">> 评估结果已保存: " + finalSavePath);
    return true;
}

bool EvaluatorPanelBase::_HasMaskInput() const {
    return _MaskSelect && !_MaskSelect->CurrentPath().trimmed().isEmpty();
}

bool EvaluatorPanelBase::_IsBoundaryOnlyRequested() const {
    return _BoundaryOnlyCheck && _BoundaryOnlyCheck->isVisible() &&
           _BoundaryOnlyCheck->isEnabled() && _BoundaryOnlyCheck->isChecked();
}

void EvaluatorPanelBase::_SetBoundaryOnlySupported(bool supported) {
    _BoundaryOnlySupported = supported;
    _UpdateBoundaryOnlyState();
}

void EvaluatorPanelBase::_UpdateBoundaryOnlyState() {
    if (!_BoundaryOnlyCheck) {
        return;
    }

    _BoundaryOnlyCheck->setVisible(_BoundaryOnlySupported);

    const bool enableBoundaryOnly = _BoundaryOnlySupported && _HasMaskInput();
    _BoundaryOnlyCheck->setEnabled(enableBoundaryOnly);

    if (!enableBoundaryOnly) {
        _BoundaryOnlyCheck->setChecked(false);
    }
}

} // namespace Panels::Evaluation
