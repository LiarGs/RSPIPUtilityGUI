#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QTextEdit>

#include "Infrastructure/Execution/ExecutionTypes.h"

class QDockWidget;
class QTimer;

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
    void OnLogMessage(const QString &msg, bool replaceLast = false);
    void OnPageExecutionStarted();
    void OnPageExecutionFinished(const Infrastructure::Execution::ExecutionResult &result);
    void OnCurrentAlgorithmChanged();

  private:
    struct PendingLogEntry {
        QString html;
        bool replaceLast = false;
    };

    void _SetupUi();
    void _InitModules();
    void _UpdateAlgorithmDescription();
    void _UpdateOutputHints();
    void _ScheduleLogFlush();
    void _FlushPendingLogs();
    void _AppendLogHtml(const QString &html);
    void _ReplaceLastLogHtml(const QString &html);

    QComboBox *_AlgoSelector = nullptr;
    QStackedWidget *_ParamStack = nullptr;
    QLineEdit *_OutputPathEdit = nullptr;
    QPushButton *_BrowseOutputBtn = nullptr;
    QPushButton *_RunBtn = nullptr;
    QTextEdit *_LogConsole = nullptr;
    QTextEdit *_DescriptionView = nullptr;
    QDockWidget *_LogDock = nullptr;
    QDockWidget *_DescriptionDock = nullptr;
    QTimer *_LogFlushTimer = nullptr;
    QList<PendingLogEntry> _PendingLogEntries;
    bool _HasActiveInlineLogBlock = false;

    QList<Pages::ModulePageBase *> _Pages;
};

} // namespace UI
