#include "ReconstructPage.h"
#include "Panels/Reconstruct/ColorBalancePanel.h"
#include "Panels/Reconstruct/IsophotePanel.h"
#include "Panels/Reconstruct/SimplePanel.h"

namespace UI::Pages {

ReconstructPage::ReconstructPage(QWidget *parent) : ModulePageBase(parent) {
    _RegisterAlgorithm(new Panels::Reconstruct::IsophotePanel(this));
    _RegisterAlgorithm(new Panels::Reconstruct::ColorBalancePanel(this));
    _RegisterAlgorithm(new Panels::Reconstruct::SimplePanel(this));
}

} // namespace UI::Pages