#pragma once
#include "ModulePageBase.h"

namespace UI::Pages {

class ColorBalancePage : public ModulePageBase {
    Q_OBJECT
  public:
    explicit ColorBalancePage(QWidget *parent = nullptr);
    QString ModuleName() const override { return "匀色处理 (Color Balance)"; }
};

} // namespace UI::Pages
