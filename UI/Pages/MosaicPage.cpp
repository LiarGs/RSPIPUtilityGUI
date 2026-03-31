#include "MosaicPage.h"
#include "Panels/Mosaic/AdaptiveColorBalancePatchPanel.h"
#include "Panels/Mosaic/AdaptivePatchPanel.h"
#include "Panels/Mosaic/AdaptiveIsophotePatchPanel.h"
#include "Panels/Mosaic/DynamicPatchPanel.h"
#include "Panels/Mosaic/ShowOverlapPanel.h"
#include "Panels/Mosaic/SimplePanel.h"

namespace UI::Pages {

MosaicPage::MosaicPage(QWidget *parent) : ModulePageBase(parent) {
    _RegisterAlgorithm(new Panels::Mosaic::SimplePanel(this));
    _RegisterAlgorithm(new Panels::Mosaic::ShowOverlapPanel(this));
    _RegisterAlgorithm(new Panels::Mosaic::DynamicPatchPanel(this));
    _RegisterAlgorithm(new Panels::Mosaic::AdaptivePatchPanel(this));
    _RegisterAlgorithm(new Panels::Mosaic::AdaptiveColorBalancePatchPanel(this));
    _RegisterAlgorithm(new Panels::Mosaic::AdaptiveIsophotePatchPanel(this));
}

} // namespace UI::Pages
