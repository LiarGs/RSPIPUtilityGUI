#include "MainWindow.h"

#include "Application/Registry/ModuleRegistry.h"
#include "Pages/ModulePageBase.h"
#include "Util/SuperDebug.hpp"

#include <QDockWidget>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaObject>
#include <QPointer>
#include <QScrollBar>
#include <QSizePolicy>
#include <QSplitter>
#include <QTextBlock>
#include <QTextCursor>
#include <QTimer>
#include <QVBoxLayout>

namespace {

constexpr int kLogFlushIntervalMs = 33;

QString PlainTextToHtml(const QString &text) {
    QString html = text.toHtmlEscaped();
    html.replace('\n', "<br>");
    return html;
}

} // namespace

namespace UI {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    _SetupUi();
    _InitModules();

    if (_AlgoSelector->count() > 0) {
        OnAlgorithmChanged(0);
    }
    resize(1280, 850);
    setWindowTitle(tr("RSPIP 工具库 GUI"));

    QPointer<MainWindow> self(this);
    SuperDebug::SetLoggerCallback([self](SuperDebug::Level level, const std::string &msg,
                                         SuperDebug::LogUpdateMode updateMode) {
        if (!self) {
            return;
        }

        QString qMsg = PlainTextToHtml(QString::fromStdString(msg));

        QString colorHtml;
        switch (level) {
        case SuperDebug::Level::Info:
            colorHtml = "#a9b7c6";
            break;
        case SuperDebug::Level::Warn:
            colorHtml = "#FFD700";
            break;
        case SuperDebug::Level::Error:
            colorHtml = "#FF4500";
            break;
        }

        QString formattedMsg = QString("<span style='color:%1;'>%2</span>").arg(colorHtml, qMsg);

        QMetaObject::invokeMethod(self, "OnLogMessage",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, formattedMsg),
                                  Q_ARG(bool, updateMode == SuperDebug::LogUpdateMode::ReplaceLast));
    });
}

MainWindow::~MainWindow() {
    _FlushPendingLogs();
    SuperDebug::SetLoggerCallback([](SuperDebug::Level, const std::string &, SuperDebug::LogUpdateMode) {});
}

void MainWindow::_SetupUi() {
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QHBoxLayout(centralWidget);

    auto *controlPanel = new QGroupBox(tr("控制面板"), this);
    controlPanel->setFixedWidth(340);
    auto *controlLayout = new QVBoxLayout(controlPanel);

    controlLayout->addWidget(new QLabel(tr("1. 功能模块:"), this));
    _AlgoSelector = new QComboBox(this);
    controlLayout->addWidget(_AlgoSelector);
    controlLayout->addSpacing(10);

    _ParamStack = new QStackedWidget(this);
    controlLayout->addWidget(_ParamStack);

    auto *outputGroup = new QGroupBox(tr("3. 结果保存 (可选)"), this);
    auto *outputLayout = new QVBoxLayout(outputGroup);
    _OutputPathEdit = new QLineEdit(this);
    _OutputPathEdit->setPlaceholderText(tr("留空则自动生成临时文件..."));
    _BrowseOutputBtn = new QPushButton(tr("选择保存路径..."), this);
    connect(_BrowseOutputBtn, &QPushButton::clicked, this, &MainWindow::OnBrowseOutput);
    outputLayout->addWidget(_OutputPathEdit);
    outputLayout->addWidget(_BrowseOutputBtn);
    controlLayout->addWidget(outputGroup);
    controlLayout->addSpacing(10);

    _RunBtn = new QPushButton(tr(">>> 执行处理 >>>"), this);
    _RunBtn->setMinimumHeight(50);
    controlLayout->addWidget(_RunBtn);

    auto *logGroup = new QGroupBox(tr("控制台日志"), this);
    auto *logLayout = new QVBoxLayout(logGroup);
    _LogConsole = new QTextEdit(this);
    _LogConsole->setReadOnly(true);
    _LogConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    logLayout->addWidget(_LogConsole);

    _LogFlushTimer = new QTimer(this);
    _LogFlushTimer->setSingleShot(true);
    connect(_LogFlushTimer, &QTimer::timeout, this, &MainWindow::_FlushPendingLogs);

    auto *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(controlPanel);
    mainSplitter->addWidget(logGroup);
    mainSplitter->setStretchFactor(1, 1);
    mainLayout->addWidget(mainSplitter);

    _LogDock = new QDockWidget(tr("算法说明"), this);
    _LogDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    _LogDock->setFeatures(QDockWidget::DockWidgetMovable |
                          QDockWidget::DockWidgetFloatable |
                          QDockWidget::DockWidgetClosable);
    _LogDock->setMinimumHeight(120);
    _DescriptionView = new QTextEdit(this);
    _DescriptionView->setReadOnly(true);
    _DescriptionView->setPlaceholderText(tr("当前算法说明将在这里显示。"));
    _DescriptionView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _LogDock->setWidget(_DescriptionView);
    addDockWidget(Qt::BottomDockWidgetArea, _LogDock);
    QTimer::singleShot(0, this, [this]() {
        if (_LogDock) {
            resizeDocks({_LogDock}, {220}, Qt::Vertical);
        }
    });

    connect(_AlgoSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::OnAlgorithmChanged);
    connect(_RunBtn, &QPushButton::clicked, this, &MainWindow::OnExecuteClicked);
}

void MainWindow::_InitModules() {
    for (const auto &module : Application::Registry::AllModules()) {
        auto *page = new Pages::ModulePageBase(module.ModuleId, this);
        _Pages.append(page);
        _ParamStack->addWidget(page);
        _AlgoSelector->addItem(module.ModuleDisplayName);

        connect(page, &ModulePageBase::LogMessage, this, [this](const QString &msg) {
            OnLogMessage(PlainTextToHtml(msg), false);
        });
        connect(page, &ModulePageBase::ExecutionStarted, this, &MainWindow::OnPageExecutionStarted);
        connect(page, &ModulePageBase::ExecutionFinished, this, &MainWindow::OnPageExecutionFinished);
        connect(page, &ModulePageBase::CurrentAlgorithmChanged, this, &MainWindow::OnCurrentAlgorithmChanged);
    }
}

void MainWindow::OnAlgorithmChanged(int index) {
    if (index >= 0 && index < _ParamStack->count()) {
        _ParamStack->setCurrentIndex(index);
        _UpdateAlgorithmDescription();
        _UpdateOutputHints();
    }
}

void MainWindow::OnBrowseOutput() {
    auto *currentPage = qobject_cast<ModulePageBase *>(_ParamStack->currentWidget());
    const bool useDirectoryMode =
        currentPage &&
        currentPage->CurrentOutputSelectionMode() ==
            Application::Registry::OutputSelectionMode::Directory;

    const QString currentPath = _OutputPathEdit ? _OutputPathEdit->text().trimmed() : QString();

    if (useDirectoryMode) {
        const QString selectedDir = QFileDialog::getExistingDirectory(
            this,
            tr("选择输出目录"),
            currentPath);
        if (!selectedDir.isEmpty()) {
            _OutputPathEdit->setText(selectedDir);
        }
        return;
    }

    QFileDialog dialog(this, tr("保存结果"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDefaultSuffix(QString());
    dialog.setNameFilters({
        tr("所有文件 (*)"),
        tr("GeoTIFF (*.tif *.tiff)"),
        tr("Text (*.txt)")
    });
    dialog.selectNameFilter(tr("所有文件 (*)"));

    if (!currentPath.isEmpty()) {
        dialog.selectFile(currentPath);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            _OutputPathEdit->setText(selectedFiles.first());
        }
    }
}

void MainWindow::OnExecuteClicked() {
    auto *currentPage = qobject_cast<ModulePageBase *>(_ParamStack->currentWidget());
    if (currentPage) {
        currentPage->Execute(_OutputPathEdit->text());
    }
}

void MainWindow::OnLogMessage(const QString &msg, bool replaceLast) {
    if (!_LogConsole) {
        return;
    }

    if (replaceLast &&
        !_PendingLogEntries.isEmpty() &&
        _PendingLogEntries.last().replaceLast) {
        _PendingLogEntries.last().html = msg;
    } else {
        _PendingLogEntries.append({msg, replaceLast});
    }

    _ScheduleLogFlush();
}

void MainWindow::_ScheduleLogFlush() {
    if (_LogFlushTimer && !_LogFlushTimer->isActive()) {
        _LogFlushTimer->start(kLogFlushIntervalMs);
    }
}

void MainWindow::_FlushPendingLogs() {
    if (!_LogConsole || _PendingLogEntries.isEmpty()) {
        return;
    }

    QList<PendingLogEntry> pendingEntries;
    pendingEntries.swap(_PendingLogEntries);

    for (const PendingLogEntry &entry : pendingEntries) {
        if (entry.replaceLast) {
            if (_HasActiveInlineLogBlock) {
                _ReplaceLastLogHtml(entry.html);
            } else {
                _AppendLogHtml(entry.html);
                _HasActiveInlineLogBlock = true;
            }
        } else {
            _AppendLogHtml(entry.html);
            _HasActiveInlineLogBlock = false;
        }
    }

    if (QScrollBar *scrollBar = _LogConsole->verticalScrollBar()) {
        scrollBar->setValue(scrollBar->maximum());
    }

    if (!_PendingLogEntries.isEmpty()) {
        _ScheduleLogFlush();
    }
}

void MainWindow::_AppendLogHtml(const QString &html) {
    if (!_LogConsole) {
        return;
    }

    auto *document = _LogConsole->document();
    if (!document) {
        return;
    }

    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::End);

    if (!document->isEmpty()) {
        cursor.insertBlock();
    }

    cursor.insertHtml(html);
}

void MainWindow::_ReplaceLastLogHtml(const QString &html) {
    if (!_LogConsole) {
        return;
    }

    auto *document = _LogConsole->document();
    if (!document || document->isEmpty()) {
        _AppendLogHtml(html);
        return;
    }

    const QTextBlock lastBlock = document->lastBlock();
    if (!lastBlock.isValid()) {
        _AppendLogHtml(html);
        return;
    }

    QTextCursor cursor(lastBlock);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertHtml(html);
}

void MainWindow::OnCurrentAlgorithmChanged() {
    _UpdateAlgorithmDescription();
    _UpdateOutputHints();
}

void MainWindow::OnPageExecutionStarted() {
    _RunBtn->setEnabled(false);
    _AlgoSelector->setEnabled(false);
    _OutputPathEdit->setEnabled(false);
    _BrowseOutputBtn->setEnabled(false);
    _RunBtn->setText(tr("处理中..."));
}

void MainWindow::OnPageExecutionFinished(const Infrastructure::Execution::ExecutionResult &result) {
    _RunBtn->setEnabled(true);
    _AlgoSelector->setEnabled(true);
    _OutputPathEdit->setEnabled(true);
    _BrowseOutputBtn->setEnabled(true);
    _RunBtn->setText(tr(">>> 执行处理 >>>"));

    const QString summary = result.Message.isEmpty()
                                ? (result.Success ? tr("任务执行完成。") : tr("任务执行失败，请查看上方日志。"))
                                : result.Message;
    const QString color = result.Success ? QStringLiteral("#7CFC00") : QStringLiteral("#FF7F50");
    OnLogMessage(QString("<span style='color:%1;'>%2</span>")
                     .arg(color, PlainTextToHtml(summary)));
}

void MainWindow::_UpdateAlgorithmDescription() {
    if (!_DescriptionView) {
        return;
    }

    auto *currentPage = qobject_cast<ModulePageBase *>(_ParamStack->currentWidget());
    if (!currentPage) {
        _DescriptionView->setPlainText(tr("未找到当前页面。"));
        return;
    }

    auto *panel = currentPage->CurrentPanel();
    if (!panel) {
        _DescriptionView->setPlainText(tr("未找到当前算法面板。"));
        return;
    }

    _DescriptionView->setPlainText(panel->AlgorithmDescription());
}

void MainWindow::_UpdateOutputHints() {
    if (!_OutputPathEdit || !_BrowseOutputBtn) {
        return;
    }

    auto *currentPage = qobject_cast<ModulePageBase *>(_ParamStack->currentWidget());
    const bool useDirectoryMode =
        currentPage &&
        currentPage->CurrentOutputSelectionMode() ==
            Application::Registry::OutputSelectionMode::Directory;

    _OutputPathEdit->setPlaceholderText(useDirectoryMode
                                            ? tr("留空则自动生成临时目录结果，或填写输出目录...")
                                            : tr("留空则自动生成临时文件..."));
    _BrowseOutputBtn->setText(useDirectoryMode ? tr("选择输出目录...") : tr("选择保存路径..."));
}

} // namespace UI
