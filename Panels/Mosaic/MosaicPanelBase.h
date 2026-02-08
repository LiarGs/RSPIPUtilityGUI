#pragma once
#include "Panels/AlgorithmPanelBase.h"

#include "Common/FileListWidget.h"
#include <QApplication>
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
#include "Basic/CloudMask.h"
#include "Basic/GeoImage.h"
#include "IO/ImageReader.h"
#include "IO/ImageSaveVisitor.h"

namespace Panels::Mosaic {

using UI::Common::FileListWidget;
/**
 * @brief 镶嵌算法面板基类 (可选)
 * @details 提取 Mosaic 类算法共有的行为（例如都需要选择影像列表）
 */
class MosaicPanelBase : public AlgorithmPanelBase {
    Q_OBJECT
  public:
    explicit MosaicPanelBase(QWidget *parent = nullptr);
    ~MosaicPanelBase() override = default;

    bool ValidateInput() const override;

  protected:
    void _SetupUi() override;
    bool _SaveResult(const RSPIP::GeoImage &result, const QString &userPath, const QString &prefix);

  protected:
    FileListWidget *_ImageSelector;
};

} // namespace Panels::Mosaic