#pragma once
#include "IAlgorithmPage.h"

class QLineEdit;
class QComboBox;

class ReconstructPage : public IAlgorithmPage {
    Q_OBJECT
public:
    explicit ReconstructPage(QWidget *parent = nullptr);
    QString moduleName() const override { return "影像重构 (Reconstruct)"; }
    void execute(const QString &savePath) override;

private slots:
    void onBrowseTarget();
    void onBrowseRefer();
    void onBrowseMask();

private:
    QComboBox *m_algoSelectCombo;
    QLineEdit *m_targetEdit;
    QLineEdit *m_referEdit;
    QLineEdit *m_maskEdit;
};
