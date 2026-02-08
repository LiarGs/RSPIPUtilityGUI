#include "Pages/ModulePageBase.h"
#include "Panels/AlgorithmPanelBase.h"

#include <QComboBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace UI::Pages {

ModulePageBase::ModulePageBase(QWidget *parent) : QWidget(parent) {
    _SetupUi();
}

void ModulePageBase::_SetupUi() {
    auto *layout = new QVBoxLayout(this);

    // 1. 算法选择区域 (Group 1)
    auto *topGroup = new QGroupBox("1. 算法选择", this);
    auto *topLayout = new QVBoxLayout(topGroup);

    _AlgoSelectCombo = new QComboBox(this);
    // 连接信号
    connect(_AlgoSelectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ModulePageBase::OnModuleChanged);

    topLayout->addWidget(_AlgoSelectCombo);
    layout->addWidget(topGroup);

    // 2. 参数设置区域 (Group 2)
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

    // 自动转发信号：Panel日志 -> Page日志
    connect(panel, &AlgorithmPanelBase::LogMessage, this, &ModulePageBase::LogMessage);
}

void ModulePageBase::OnModuleChanged(int index) {
    if (index >= 0 && index < _PanelStack->count()) {
        _PanelStack->setCurrentIndex(index);
    }
}

void ModulePageBase::Execute(const QString &savePath) {
    auto *currentPanel = CurrentPanel();
    if (currentPanel && currentPanel->ValidateInput()) {
        currentPanel->Run(savePath);
    }
}

} // namespace UI::Pages
