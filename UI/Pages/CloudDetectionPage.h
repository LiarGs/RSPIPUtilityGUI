#pragma once
#include "ModulePageBase.h"

namespace UI::Pages {

class CloudDetectionPage : public ModulePageBase {
    Q_OBJECT
  public:
    explicit CloudDetectionPage(QWidget *parent = nullptr);
    QString ModuleName() const override { return "云检测 (Cloud Detection)"; }
};

} // namespace UI::Pages
