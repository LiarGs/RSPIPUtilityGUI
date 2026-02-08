#include "ColorBalancePage.h"
#include "Panels/ColorBalance/MatchStatisticsPanel.h"

namespace UI::Pages {

ColorBalancePage::ColorBalancePage(QWidget *parent) : ModulePageBase(parent) {
    _RegisterAlgorithm(new Panels::ColorBalance::MatchStatisticsPanel(this));
}

} // namespace UI::Pages
