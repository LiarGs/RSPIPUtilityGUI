#pragma once

#include "Application/Registry/ModuleRegistry.h"
#include "Infrastructure/Execution/ExecutionTypes.h"
#include "Panels/AlgorithmPanelBase.h"

#include <QComboBox>
#include <QStackedWidget>
#include <QWidget>

#include <vector>

namespace Infrastructure::Execution {
class TaskExecutor;
}

namespace UI::Pages {

using Panels::AlgorithmPanelBase;

class ModulePageBase : public QWidget {
    Q_OBJECT
  public:
    explicit ModulePageBase(const QString &moduleId,
                            QWidget *parent = nullptr);
    ~ModulePageBase() override;

    QString ModuleName() const;
    AlgorithmPanelBase *CurrentPanel() const;
    Application::Registry::OutputSelectionMode CurrentOutputSelectionMode() const;
    QString ModuleId() const;
    void Execute(const QString &savePath);

  signals:
    void LogMessage(const QString &msg);
    void ExecutionStarted();
    void ExecutionFinished(const Infrastructure::Execution::ExecutionResult &result);
    void CurrentAlgorithmChanged();

  protected:
    virtual void _SetupUi();

  private slots:
    void OnModuleChanged(int index);

  private:
    struct RegisteredAlgorithm {
        Application::Registry::AlgorithmDescriptor Descriptor;
        AlgorithmPanelBase *Panel = nullptr;
    };

    void _RegisterAlgorithms();
    void _FinalizeTask(const Infrastructure::Execution::ExecutionResult &result);

    Application::Registry::ModuleDescriptor _ModuleDescriptor;
    std::vector<RegisteredAlgorithm> _Algorithms;

    QComboBox *_AlgoSelectCombo = nullptr;
    QStackedWidget *_PanelStack = nullptr;
    Infrastructure::Execution::TaskExecutor *_TaskExecutor = nullptr;
    bool _IsExecuting = false;
};

} // namespace UI::Pages
