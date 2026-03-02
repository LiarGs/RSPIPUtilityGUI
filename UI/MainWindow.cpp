#include "MainWindow.h"
#include "Pages/ColorBalancePage.h"
#include "Pages/MosaicPage.h"
#include "Pages/ReconstructPage.h"
#include "Util/SuperDebug.hpp"
#include <QDockWidget>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaObject>
#include <QPointer>
#include <QSplitter>
#include <QVBoxLayout>

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
    SuperDebug::SetLoggerCallback([self](SuperDebug::Level level, const std::string &msg) {
        if (!self) {
            return;
        }

        QString qMsg = QString::fromStdString(msg);

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
                                  Q_ARG(QString, formattedMsg));
    });
}

MainWindow::~MainWindow() {
    SuperDebug::SetLoggerCallback([](SuperDebug::Level, const std::string &) {});
}

void MainWindow::_SetupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QGroupBox *controlPanel = new QGroupBox(tr("控制面板"), this);
    controlPanel->setFixedWidth(340);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    controlLayout->addWidget(new QLabel(tr("1. 功能模块:"), this));
    _AlgoSelector = new QComboBox(this);
    controlLayout->addWidget(_AlgoSelector);
    controlLayout->addSpacing(10);

    _ParamStack = new QStackedWidget(this);
    controlLayout->addWidget(_ParamStack);

    QGroupBox *outputGroup = new QGroupBox(tr("3. 结果保存 (可选)"), this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    _OutputPathEdit = new QLineEdit(this);
    _OutputPathEdit->setPlaceholderText(tr("留空则自动生成临时文件..."));
    QPushButton *browseOutBtn = new QPushButton(tr("选择保存路径..."), this);
    connect(browseOutBtn, &QPushButton::clicked, this, &MainWindow::OnBrowseOutput);
    outputLayout->addWidget(_OutputPathEdit);
    outputLayout->addWidget(browseOutBtn);
    controlLayout->addWidget(outputGroup);
    controlLayout->addSpacing(10);

    _RunBtn = new QPushButton(tr(">>> 执行处理 >>>"), this);
    _RunBtn->setMinimumHeight(50);
    controlLayout->addWidget(_RunBtn);

    QGroupBox *displayGroup = new QGroupBox(tr("处理结果展示"), this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    Q_UNUSED(displayLayout);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(controlPanel);
    mainSplitter->addWidget(displayGroup);
    mainSplitter->setStretchFactor(1, 1);
    mainLayout->addWidget(mainSplitter);

    QDockWidget *logDock = new QDockWidget(tr("控制台日志"), this);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    _LogConsole = new QTextEdit(this);
    _LogConsole->setReadOnly(true);
    logDock->setWidget(_LogConsole);
    addDockWidget(Qt::BottomDockWidgetArea, logDock);

    connect(_AlgoSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::OnAlgorithmChanged);
    connect(_RunBtn, &QPushButton::clicked, this, &MainWindow::OnExecuteClicked);
}

void MainWindow::_InitModules() {
    QList<ModulePageBase *> pages;
    pages << new Pages::MosaicPage(this);
    pages << new Pages::ReconstructPage(this);
    pages << new Pages::ColorBalancePage(this);

    for (auto page : pages) {
        _Pages.append(page);
        _ParamStack->addWidget(page);
        _AlgoSelector->addItem(page->ModuleName());

        connect(page, &ModulePageBase::LogMessage, this, &MainWindow::OnLogMessage);
        connect(page, &ModulePageBase::ExecutionStarted, this, &MainWindow::OnPageExecutionStarted);
        connect(page, &ModulePageBase::ExecutionFinished, this, &MainWindow::OnPageExecutionFinished);
    }
}

void MainWindow::OnAlgorithmChanged(int index) {
    if (index >= 0 && index < _ParamStack->count()) {
        _ParamStack->setCurrentIndex(index);
    }
}

void MainWindow::OnBrowseOutput() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存结果"), "",
                                                    tr("GeoTIFF (*.tif);;Text (*.txt)"));
    if (!fileName.isEmpty()) {
        _OutputPathEdit->setText(fileName);
    }
}

void MainWindow::OnExecuteClicked() {
    ModulePageBase *currentPage = dynamic_cast<ModulePageBase *>(_ParamStack->currentWidget());
    if (currentPage) {
        currentPage->Execute(_OutputPathEdit->text());
    }
}

void MainWindow::OnLogMessage(const QString &msg) {
    _LogConsole->append(msg);
}

void MainWindow::OnPageExecutionStarted() {
    _RunBtn->setEnabled(false);
    _AlgoSelector->setEnabled(false);
    _RunBtn->setText(tr("处理中..."));
}

void MainWindow::OnPageExecutionFinished(bool success) {
    _RunBtn->setEnabled(true);
    _AlgoSelector->setEnabled(true);
    _RunBtn->setText(tr(">>> 执行处理 >>>"));

    if (success) {
        OnLogMessage("<span style='color:#7CFC00;'>任务执行完成。</span>");
    } else {
        OnLogMessage("<span style='color:#FF7F50;'>任务执行失败，请查看上方日志。</span>");
    }
}

} // namespace UI
