#include "Application/Registry/ModuleRegistry.h"

#include "Panels/CloudDetection/PixelThresholdPanel.h"
#include "Panels/ColorBalance/MatchStatisticsPanel.h"
#include "Panels/Evaluation/BoundaryGradientEvaluatorPanel.h"
#include "Panels/Evaluation/PSNREvaluatorPanel.h"
#include "Panels/Evaluation/RMSEEvaluatorPanel.h"
#include "Panels/Evaluation/SSIMEvaluatorPanel.h"
#include "Panels/Mosaic/AdaptiveColorBalancePatchPanel.h"
#include "Panels/Mosaic/AdaptiveIsophotePatchPanel.h"
#include "Panels/Mosaic/AdaptivePatchPanel.h"
#include "Panels/Mosaic/ShowOverlapPanel.h"
#include "Panels/Mosaic/SimplePanel.h"
#include "Panels/Preprocess/GeoCoordinateAlignPanel.h"
#include "Panels/Reconstruct/ColorBalancePanel.h"
#include "Panels/Reconstruct/IsophotePanel.h"
#include "Panels/Reconstruct/SimplePanel.h"

#include <algorithm>

namespace Application::Registry {

namespace {

const std::vector<ModuleDescriptor> kModules = {
    {QStringLiteral("preprocess"), QStringLiteral("预处理 (Preprocess)"), 10},
    {QStringLiteral("mosaic"), QStringLiteral("影像镶嵌 (Mosaic)"), 20},
    {QStringLiteral("reconstruct"), QStringLiteral("影像重构 (Reconstruct)"), 30},
    {QStringLiteral("color_balance"), QStringLiteral("匀色处理 (Color Balance)"), 40},
    {QStringLiteral("cloud_detection"), QStringLiteral("云检测 (Cloud Detection)"), 50},
    {QStringLiteral("evaluation"), QStringLiteral("质量评估 (Evaluation)"), 60},
};

const std::vector<AlgorithmDescriptor> kAlgorithms = {
    {QStringLiteral("preprocess"), QStringLiteral("geo_coordinate_align"), QStringLiteral("GeoCoordinateAlign"), OutputSelectionMode::Directory, 10, [](QWidget *parent) { return new Panels::Preprocess::GeoCoordinateAlignPanel(parent); }},

    {QStringLiteral("mosaic"), QStringLiteral("simple"), QStringLiteral("Simple"), OutputSelectionMode::FilePath, 10, [](QWidget *parent) { return new Panels::Mosaic::SimplePanel(parent); }},
    {QStringLiteral("mosaic"), QStringLiteral("show_overlap"), QStringLiteral("ShowOverlap"), OutputSelectionMode::FilePath, 20, [](QWidget *parent) { return new Panels::Mosaic::ShowOverlapPanel(parent); }},
    {QStringLiteral("mosaic"), QStringLiteral("adaptive_patch"), QStringLiteral("AdaptivePatch"), OutputSelectionMode::FilePath, 30, [](QWidget *parent) { return new Panels::Mosaic::AdaptivePatchPanel(parent); }},
    {QStringLiteral("mosaic"), QStringLiteral("adaptive_color_balance_patch"), QStringLiteral("AdaptiveColorBalancePatch"), OutputSelectionMode::FilePath, 40, [](QWidget *parent) { return new Panels::Mosaic::AdaptiveColorBalancePatchPanel(parent); }},
    {QStringLiteral("mosaic"), QStringLiteral("adaptive_isophote_patch"), QStringLiteral("AdaptiveIsophotePatch"), OutputSelectionMode::FilePath, 50, [](QWidget *parent) { return new Panels::Mosaic::AdaptiveIsophotePatchPanel(parent); }},

    {QStringLiteral("reconstruct"), QStringLiteral("isophote_constrain"), QStringLiteral("IsophoteConstrain (等照度线约束)"), OutputSelectionMode::FilePath, 10, [](QWidget *parent) { return new Panels::Reconstruct::IsophotePanel(parent); }},
    {QStringLiteral("reconstruct"), QStringLiteral("color_balance_reconstruct"), QStringLiteral("ColorBalanceReconstruct (匀色重构)"), OutputSelectionMode::FilePath, 20, [](QWidget *parent) { return new Panels::Reconstruct::ColorBalancePanel(parent); }},
    {QStringLiteral("reconstruct"), QStringLiteral("simple_reconstruct"), QStringLiteral("SimpleReconstruct"), OutputSelectionMode::FilePath, 30, [](QWidget *parent) { return new Panels::Reconstruct::SimplePanel(parent); }},

    {QStringLiteral("color_balance"), QStringLiteral("match_statistics"), QStringLiteral("MatchStatistics"), OutputSelectionMode::Directory, 10, [](QWidget *parent) { return new Panels::ColorBalance::MatchStatisticsPanel(parent); }},

    {QStringLiteral("cloud_detection"), QStringLiteral("pixel_threshold"), QStringLiteral("PixelThreshold"), OutputSelectionMode::Directory, 10, [](QWidget *parent) { return new Panels::CloudDetection::PixelThresholdPanel(parent); }},

    {QStringLiteral("evaluation"), QStringLiteral("psnr"), QStringLiteral("PSNREvaluator"), OutputSelectionMode::FilePath, 10, [](QWidget *parent) { return new Panels::Evaluation::PSNREvaluatorPanel(parent); }},
    {QStringLiteral("evaluation"), QStringLiteral("ssim"), QStringLiteral("SSIMEvaluator"), OutputSelectionMode::FilePath, 20, [](QWidget *parent) { return new Panels::Evaluation::SSIMEvaluatorPanel(parent); }},
    {QStringLiteral("evaluation"), QStringLiteral("rmse"), QStringLiteral("RMSEEvaluator"), OutputSelectionMode::FilePath, 30, [](QWidget *parent) { return new Panels::Evaluation::RMSEEvaluatorPanel(parent); }},
    {QStringLiteral("evaluation"), QStringLiteral("boundary_gradient"), QStringLiteral("BoundaryGradientEvaluator"), OutputSelectionMode::FilePath, 40, [](QWidget *parent) { return new Panels::Evaluation::BoundaryGradientEvaluatorPanel(parent); }},
};

} // namespace

const std::vector<ModuleDescriptor> &AllModules() {
    return kModules;
}

std::optional<ModuleDescriptor> FindModule(const QString &moduleId) {
    const auto it = std::find_if(
        kModules.begin(),
        kModules.end(),
        [&moduleId](const ModuleDescriptor &descriptor) { return descriptor.ModuleId == moduleId; });
    if (it == kModules.end()) {
        return std::nullopt;
    }
    return *it;
}

std::vector<AlgorithmDescriptor> AlgorithmsForModule(const QString &moduleId) {
    std::vector<AlgorithmDescriptor> results;
    for (const auto &descriptor : kAlgorithms) {
        if (descriptor.ModuleId == moduleId) {
            results.push_back(descriptor);
        }
    }

    std::sort(results.begin(),
              results.end(),
              [](const AlgorithmDescriptor &lhs, const AlgorithmDescriptor &rhs) {
                  return lhs.SortOrder < rhs.SortOrder;
              });
    return results;
}

} // namespace Application::Registry
