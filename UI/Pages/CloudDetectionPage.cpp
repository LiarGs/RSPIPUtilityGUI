#include "CloudDetectionPage.h"
#include "Panels/CloudDetection/PixelThresholdPanel.h"

namespace UI::Pages {

CloudDetectionPage::CloudDetectionPage(QWidget *parent) : ModulePageBase(parent) {
    _RegisterAlgorithm(new Panels::CloudDetection::PixelThresholdPanel(this));
}

} // namespace UI::Pages
