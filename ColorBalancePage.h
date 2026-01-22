#pragma once
#include "IAlgorithmPage.h"

class QLineEdit;
class QComboBox;

class ColorBalancePage : public IAlgorithmPage {
    Q_OBJECT
public:
    explicit ColorBalancePage(QWidget *parent = nullptr);
    QString moduleName() const override { return "匀色处理 (Color Balance)"; }

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
