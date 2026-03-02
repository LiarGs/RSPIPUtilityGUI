#include "Pages/ModulePageBase.h"
#include "Panels/AlgorithmPanelBase.h"

#include <QComboBox>
#include <QGroupBox>
#include <QMetaObject>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace UI::Pages {

ModulePageBase::ModulePageBase(QWidget *parent) : QWidget(parent) {
    _SetupUi();
}

ModulePageBase::~ModulePageBase() {
    if (_TaskThread) {
        _TaskThread->quit();
        _TaskThread->wait();
    }
}

void ModulePageBase::_SetupUi() {
    auto *layout = new QVBoxLayout(this);

    auto *topGroup = new QGroupBox("1. 算法选择", this);
    auto *topLayout = new QVBoxLayout(topGroup);

    _AlgoSelectCombo = new QComboBox(this);
    connect(_AlgoSelectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ModulePageBase::OnModuleChanged);

    topLayout->addWidget(_AlgoSelectCombo);
    layout->addWidget(topGroup);

    auto *paramGroup = new QGroupBox("2. 参数设置", this);
    auto *paramLayout = new QVBoxLayout(paramGroup);

    _PanelStack = new QStackedWidget(this);
    paramLayout->addWidget(_PanelStack);
    layout->addWidget(paramGroup);

    layout->addStretch();
}

AlgorithmPanelBase *ModulePageBase::CurrentPanel() const {
    return qobject_cast<AlgorithmPanelBase *>(_PanelStack->currentWidget());
}

void ModulePageBase::_RegisterAlgorithm(AlgorithmPanelBase *panel) {
    if (!panel)
        return;

    auto name = panel->AlgorithmName();

    _AlgoSelectCombo->addItem(name, name);
    _PanelStack->addWidget(panel);

    connect(panel, &AlgorithmPanelBase::LogMessage, this, &ModulePageBase::LogMessage);
}

void ModulePageBase::OnModuleChanged(int index) {
    if (index >= 0 && index < _PanelStack->count()) {
        _PanelStack->setCurrentIndex(index);
    }
}

void ModulePageBase::Execute(const QString &savePath) {
    if (_IsExecuting) {
        emit LogMessage("任务正在执行中，请稍候...");
        return;
    }

    auto *currentPanel = CurrentPanel();
    if (!currentPanel) {
        emit LogMessage("错误: 未找到当前算法面板。");
        return;
    }

    if (!currentPanel->ValidateInput()) {
        return;
    }

    auto task = currentPanel->BuildTask(savePath);
    if (!task) {
        emit LogMessage("错误: 构建后台任务失败。");
        return;
    }

    _IsExecuting = true;
    setEnabled(false);
    emit ExecutionStarted();

    auto *thread = QThread::create([this, task = std::move(task)]() mutable {
        bool success = false;
        try {
            success = task ? task() : false;
        } catch (...) {
            success = false;
        }

        QMetaObject::invokeMethod(this, [this, success]() { _FinalizeTask(success); }, Qt::QueuedConnection);
    });

    thread->setParent(this);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    _TaskThread = thread;
    thread->start();
}

void ModulePageBase::_FinalizeTask(bool success) {
    _IsExecuting = false;
    _TaskThread = nullptr;
    setEnabled(true);
    emit ExecutionFinished(success);
}

} // namespace UI::Pages
