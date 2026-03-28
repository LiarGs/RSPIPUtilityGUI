#pragma once

#include "EvaluatorPanelBase.h"

class QDoubleSpinBox;

namespace Panels::Evaluation {

class SSIMEvaluatorPanel : public EvaluatorPanelBase {
    Q_OBJECT
  public:
    explicit SSIMEvaluatorPanel(QWidget *parent = nullptr);
    ~SSIMEvaluatorPanel() override = default;

    QString AlgorithmName() const override { return "SSIMEvaluator (结构相似性)"; }
    QString AlgorithmDescription() const override;

    std::function<bool()> BuildTask(const QString &globalSavePath) override;

  private:
    QDoubleSpinBox *_K1Spin = nullptr;
    QDoubleSpinBox *_K2Spin = nullptr;
};

} // namespace Panels::Evaluation
