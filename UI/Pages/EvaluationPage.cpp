#include "EvaluationPage.h"

#include "Panels/Evaluation/PSNREvaluatorPanel.h"
#include "Panels/Evaluation/RMSEEvaluatorPanel.h"
#include "Panels/Evaluation/SSIMEvaluatorPanel.h"

namespace UI::Pages {

EvaluationPage::EvaluationPage(QWidget *parent)
    : ModulePageBase(parent) {
    _RegisterAlgorithm(new Panels::Evaluation::PSNREvaluatorPanel(this));
    _RegisterAlgorithm(new Panels::Evaluation::SSIMEvaluatorPanel(this));
    _RegisterAlgorithm(new Panels::Evaluation::RMSEEvaluatorPanel(this));
}

} // namespace UI::Pages
