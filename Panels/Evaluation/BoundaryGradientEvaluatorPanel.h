#pragma once

#include "EvaluatorPanelBase.h"

namespace Panels::Evaluation {

class BoundaryGradientEvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit BoundaryGradientEvaluatorPanel(QWidget *parent = nullptr);
    ~BoundaryGradientEvaluatorPanel() override = default;

    QString AlgorithmDescription() const override;

    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Evaluation
