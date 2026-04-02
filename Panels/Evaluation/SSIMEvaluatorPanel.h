#pragma once

#include "EvaluatorPanelBase.h"

class QDoubleSpinBox;

namespace Panels::Evaluation {

class SSIMEvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit SSIMEvaluatorPanel(QWidget *parent = nullptr);
    ~SSIMEvaluatorPanel() override = default;

    QString AlgorithmDescription() const override;

    std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const override;

  private:
    QDoubleSpinBox *_K1Spin = nullptr;
    QDoubleSpinBox *_K2Spin = nullptr;
};

} // namespace Panels::Evaluation
