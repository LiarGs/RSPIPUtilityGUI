#include "MainWindow.h"
#include "MosaicPage.h"
#include "ReconstructPage.h"
#include "ColorBalancePage.h"
#include "Util/SuperDebug.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QDockWidget>
#include <QLabel>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    initAlgorithms();

    if (m_algoSelector->count() > 0) {
        onAlgorithmChanged(0);
    }
    resize(1280, 850);
    setWindowTitle(tr("RSPIP 工具库 GUI 演示系统"));

    // [新增] 注册 SuperDebug 的日志回调
    SuperDebug::SetLoggerCallback([this](SuperDebug::Level level, const std::string& msg) {
        // 1. 转换字符串
        QString qMsg = QString::fromStdString(msg);

        // 2. 根据日志等级设置颜色 (HTML 格式)
        QString colorHtml;
        switch(level) {
        case SuperDebug::Level::Info:  colorHtml = "#a9b7c6"; break; // 默认灰白
        case SuperDebug::Level::Warn:  colorHtml = "#FFD700"; break; // 金黄色
        case SuperDebug::Level::Error: colorHtml = "#FF4500"; break; // 橙红色
        }

        // 3. 格式化为 HTML
        QString formattedMsg = QString("<span style='color:%1;'>%2</span>").arg(colorHtml, qMsg);

        // 4. 线程安全地调用 onLogMessage
        // 注意：这里必须用 invokeMethod 而不是直接调用 this->onLogMessage，
        // 因为这个 lambda 可能会在算法线程中被执行。
        QMetaObject::invokeMethod(this, "onLogMessage",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, formattedMsg));
    });
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // --- 1. 左侧控制面板 ---
    QGroupBox *controlPanel = new QGroupBox(tr("控制面板"), this);
    controlPanel->setFixedWidth(340);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    controlLayout->addWidget(new QLabel(tr("1. 功能模块:"), this));
    m_algoSelector = new QComboBox(this);
    controlLayout->addWidget(m_algoSelector);
    controlLayout->addSpacing(10);

    m_paramStack = new QStackedWidget(this);
    controlLayout->addWidget(m_paramStack);

    QGroupBox *outputGroup = new QGroupBox(tr("3. 结果保存 (可选)"), this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    m_outputPathEdit = new QLineEdit(this);
    m_outputPathEdit->setPlaceholderText(tr("留空则自动生成临时文件..."));
    QPushButton *browseOutBtn = new QPushButton(tr("选择保存路径..."), this);
    connect(browseOutBtn, &QPushButton::clicked, this, &MainWindow::onBrowseOutput);
    outputLayout->addWidget(m_outputPathEdit);
    outputLayout->addWidget(browseOutBtn);
    controlLayout->addWidget(outputGroup);
    controlLayout->addSpacing(10);

    m_runBtn = new QPushButton(tr(">>> 执行处理 >>>"), this);
    m_runBtn->setMinimumHeight(50);
    controlLayout->addWidget(m_runBtn);

    // --- 2. 右侧结果显示区 (使用 ResultViewer) ---
    QGroupBox *displayGroup = new QGroupBox(tr("处理结果展示"), this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);

    // [修改]: 实例化 ResultViewer
    m_resultViewer = new ResultViewer(this);
    displayLayout->addWidget(m_resultViewer);

    // --- 3. 布局组装 ---
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(controlPanel);
    mainSplitter->addWidget(displayGroup);
    mainSplitter->setStretchFactor(1, 1);
    mainLayout->addWidget(mainSplitter);

    // --- 4. 日志 ---
    QDockWidget *logDock = new QDockWidget(tr("控制台日志"), this);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    m_logConsole = new QTextEdit(this);
    m_logConsole->setReadOnly(true);
    logDock->setWidget(m_logConsole);
    addDockWidget(Qt::BottomDockWidgetArea, logDock);

    connect(m_algoSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgorithmChanged);
    connect(m_runBtn, &QPushButton::clicked, this, &MainWindow::onRunClicked);
}

void MainWindow::initAlgorithms() {
    QList<IAlgorithmPage*> pages;
    pages << new MosaicPage(this);
    pages << new ReconstructPage(this);
    pages << new ColorBalancePage(this);

    for (auto page : pages) {
        m_pages.append(page);
        m_paramStack->addWidget(page);
        m_algoSelector->addItem(page->moduleName());

        connect(page, &IAlgorithmPage::logMessage, this, &MainWindow::onLogMessage);

        connect(page, &IAlgorithmPage::resultFileReady, this, &MainWindow::onResultFileReady);
    }
}

void MainWindow::onAlgorithmChanged(int index) {
    if (index >= 0 && index < m_paramStack->count()) {
        m_paramStack->setCurrentIndex(index);
    }
}

void MainWindow::onBrowseOutput() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存结果"), "",
                                                    tr("GeoTIFF (*.tif);;Text (*.txt)"));
    if (!fileName.isEmpty()) {
        m_outputPathEdit->setText(fileName);
    }
}

void MainWindow::onRunClicked() {
    IAlgorithmPage *currentPage = dynamic_cast<IAlgorithmPage*>(m_paramStack->currentWidget());
    if (currentPage) {
        // 如果用户没填路径，传入空字符串，ColorBalancePage 内部会自动处理成临时文件
        currentPage->execute(m_outputPathEdit->text());
    }
}

void MainWindow::onLogMessage(const QString &msg) {
    m_logConsole->append(msg);
}

void MainWindow::onResultFileReady(const QString &filePath) {
    m_logConsole->append(tr(">> 准备预览结果文件: %1").arg(filePath));
    // 调用查看器加载文件
    m_resultViewer->loadResult(filePath);
}
