#pragma once
#include "ModulePageBase.h"

namespace UI::Pages {

class MosaicPage : public ModulePageBase {
    Q_OBJECT
  public:
    explicit MosaicPage(QWidget *parent = nullptr);
    QString ModuleName() const override { return "影像镶嵌 (Mosaic)"; }
};

} // namespace UI::Pages
