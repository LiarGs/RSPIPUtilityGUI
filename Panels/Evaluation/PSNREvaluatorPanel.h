#pragma once

#include "EvaluatorPanelBase.h"

namespace Panels::Evaluation {

class PSNREvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit PSNREvaluatorPanel(QWidget *parent = nullptr)
        : EvaluatorPanelBase(parent) {}

    QString AlgorithmName() const override { return "PSNREvaluator (峰值信噪比)"; }
    QString AlgorithmDescription() const override;

    std::function<bool()> BuildTask(const QString &globalSavePath) override;
};

} // namespace Panels::Evaluation
