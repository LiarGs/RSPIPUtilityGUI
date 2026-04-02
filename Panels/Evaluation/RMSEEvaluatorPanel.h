#pragma once

#include "EvaluatorPanelBase.h"

namespace Panels::Evaluation {

class RMSEEvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit RMSEEvaluatorPanel(QWidget *parent = nullptr);
    ~RMSEEvaluatorPanel() override = default;

    QString AlgorithmDescription() const override;

    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;
};

} // namespace Panels::Evaluation
