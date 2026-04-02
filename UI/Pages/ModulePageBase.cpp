#include "Pages/ModulePageBase.h"

#include "Infrastructure/Execution/TaskExecutor.h"

#include <QComboBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <stdexcept>

namespace UI::Pages {

ModulePageBase::ModulePageBase(const QString &moduleId, QWidget *parent)
    : QWidget(parent) {
    const auto descriptor = Application::Registry::FindModule(moduleId);
    if (!descriptor) {
        throw std::runtime_error(QString("未注册的模块: %1").arg(moduleId).toStdString());
    }

    _ModuleDescriptor = *descriptor;
    _SetupUi();
    _RegisterAlgorithms();

    _TaskExecutor = new Infrastructure::Execution::TaskExecutor(this);
    connect(_TaskExecutor, &Infrastructure::Execution::TaskExecutor::TaskStarted,
            this, [this]() { emit ExecutionStarted(); });
    connect(_TaskExecutor, &Infrastructure::Execution::TaskExecutor::TaskFinished,
            this, &ModulePageBase::_FinalizeTask);
}

ModulePageBase::~ModulePageBase() = default;

QString ModulePageBase::ModuleName() const {
    return _ModuleDescriptor.ModuleDisplayName;
}

QString ModulePageBase::ModuleId() const {
    return _ModuleDescriptor.ModuleId;
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

void ModulePageBase::_RegisterAlgorithms() {
    const auto algorithms = Application::Registry::AlgorithmsForModule(_ModuleDescriptor.ModuleId);
    for (const auto &descriptor : algorithms) {
        auto *panel = descriptor.PanelFactory ? descriptor.PanelFactory(this) : nullptr;
        if (!panel) {
            continue;
        }

        _Algorithms.push_back({descriptor, panel});
        _AlgoSelectCombo->addItem(descriptor.AlgorithmDisplayName, descriptor.AlgorithmId);
        _PanelStack->addWidget(panel);
    }

    if (!_Algorithms.empty()) {
        emit CurrentAlgorithmChanged();
    }
}

AlgorithmPanelBase *ModulePageBase::CurrentPanel() const {
    return qobject_cast<AlgorithmPanelBase *>(_PanelStack->currentWidget());
}

Application::Registry::OutputSelectionMode ModulePageBase::CurrentOutputSelectionMode() const {
    const int index = _PanelStack ? _PanelStack->currentIndex() : -1;
    if (index < 0 || index >= static_cast<int>(_Algorithms.size())) {
        return Application::Registry::OutputSelectionMode::FilePath;
    }
    return _Algorithms[static_cast<size_t>(index)].Descriptor.OutputMode;
}

void ModulePageBase::OnModuleChanged(int index) {
    if (index >= 0 && index < _PanelStack->count()) {
        _PanelStack->setCurrentIndex(index);
        emit CurrentAlgorithmChanged();
    }
}

void ModulePageBase::Execute(const QString &savePath) {
    if (_IsExecuting || (_TaskExecutor && _TaskExecutor->IsRunning())) {
        emit LogMessage("任务正在执行中，请稍候...");
        return;
    }

    auto *currentPanel = CurrentPanel();
    if (!currentPanel) {
        emit LogMessage("错误: 未找到当前算法面板。");
        return;
    }

    const auto validationIssue = currentPanel->ValidateInput();
    if (validationIssue) {
        QMessageBox::warning(this, validationIssue->Title, validationIssue->Message);
        emit LogMessage(QString("%1: %2").arg(validationIssue->Title, validationIssue->Message));
        return;
    }

    auto request = currentPanel->CollectRequest(savePath);
    if (!request) {
        emit LogMessage("错误: 构建算法请求失败。");
        return;
    }

    _IsExecuting = true;
    setEnabled(false);

    const bool started = _TaskExecutor->Start(
        std::move(request),
        Infrastructure::Execution::ExecutionLogSink([this](const QString &msg) { emit LogMessage(msg); }));
    if (!started) {
        _IsExecuting = false;
        setEnabled(true);
        emit LogMessage("错误: 后台任务启动失败。");
    }
}

void ModulePageBase::_FinalizeTask(const Infrastructure::Execution::ExecutionResult &result) {
    _IsExecuting = false;
    setEnabled(true);
    emit ExecutionFinished(result);
}

} // namespace UI::Pages
