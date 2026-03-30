#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>

class QDockWidget;

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
    void OnCurrentAlgorithmChanged();

  private:
    void _SetupUi();
    void _InitModules();
    void _UpdateAlgorithmDescription();
    void _UpdateOutputHints();

    QComboBox *_AlgoSelector = nullptr;
    QStackedWidget *_ParamStack = nullptr;
    QLineEdit *_OutputPathEdit = nullptr;
    QPushButton *_BrowseOutputBtn = nullptr;
    QPushButton *_RunBtn = nullptr;
    QTextEdit *_LogConsole = nullptr;
    QTextEdit *_DescriptionView = nullptr;
    QDockWidget *_LogDock = nullptr;
    QDockWidget *_DescriptionDock = nullptr;

    QList<Pages::ModulePageBase *> _Pages;
};

} // namespace UI
