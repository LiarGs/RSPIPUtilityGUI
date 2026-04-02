#pragma once

#include "EvaluatorPanelBase.h"

namespace Panels::Evaluation {

class PSNREvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit PSNREvaluatorPanel(QWidget *parent = nullptr)
        : EvaluatorPanelBase(parent) {}

    QString AlgorithmDescription() const override;

    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Evaluation
