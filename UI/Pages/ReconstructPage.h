#pragma once
#include "ModulePageBase.h"

namespace UI::Pages {

class ReconstructPage : public ModulePageBase {
    Q_OBJECT
  public:
    explicit ReconstructPage(QWidget *parent = nullptr);
    QString ModuleName() const override { return "影像重构 (Reconstruct)"; }
};

} // namespace UI::Pages
