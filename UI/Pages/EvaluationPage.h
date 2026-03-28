#pragma once

#include "ModulePageBase.h"

namespace UI::Pages {

class EvaluationPage : public ModulePageBase {
    Q_OBJECT
  public:
    explicit EvaluationPage(QWidget *parent = nullptr);
    QString ModuleName() const override { return "质量评估 (Evaluation)"; }
};

} // namespace UI::Pages
