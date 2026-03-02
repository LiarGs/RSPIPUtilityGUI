#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>

namespace UI {

namespace Pages {
class ModulePageBase;
}

using Pages::ModulePageBase;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void OnAlgorithmChanged(int index);
    void OnBrowseOutput();
    void OnExecuteClicked();
    void OnLogMessage(const QString &msg);
    void OnPageExecutionStarted();
    void OnPageExecutionFinished(bool success);

  private:
    void _SetupUi();
    void _InitModules();

    QComboBox *_AlgoSelector = nullptr;
    QStackedWidget *_ParamStack = nullptr;
    QLineEdit *_OutputPathEdit = nullptr;
    QPushButton *_RunBtn = nullptr;
    QTextEdit *_LogConsole = nullptr;

    QList<Pages::ModulePageBase *> _Pages;
};

} // namespace UI
