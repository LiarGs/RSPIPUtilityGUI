#pragma once

#include "EvaluatorPanelBase.h"

namespace Panels::Evaluation {

class RMSEEvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit RMSEEvaluatorPanel(QWidget *parent = nullptr);
    ~RMSEEvaluatorPanel() override = default;

    QString AlgorithmName() const override { return "RMSEEvaluator (均方根误差)"; }
    QString AlgorithmDescription() const override;

    std::function<bool()> BuildTask(const QString &globalSavePath) override;
};

} // namespace Panels::Evaluation
