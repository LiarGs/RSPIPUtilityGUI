#pragma once

#include "Panels/AlgorithmPanelBase.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QThread>
#include <QWidget>

namespace UI::Pages {

using Panels::AlgorithmPanelBase;

/**
 * @brief 算法页面通用基类 (Template Method Pattern)
 * @details 负责管理算法选择器(ComboBox)和参数面板栈(StackedWidget)的通用逻辑。
 * 子类只需要在构造函数中调用 _RegisterAlgorithm 添加面板即可。
 */
class ModulePageBase : public QWidget {
    Q_OBJECT
  public:
    explicit ModulePageBase(QWidget *parent = nullptr);
    virtual ~ModulePageBase() override;

    virtual QString ModuleName() const = 0;
    AlgorithmPanelBase *CurrentPanel() const;
    virtual void Execute(const QString &savePath);

  signals:
    void LogMessage(const QString &msg);
    void ExecutionStarted();
    void ExecutionFinished(bool success);

  protected:
    virtual void _SetupUi();

    /**
     * @brief 注册算法面板
     * @param panel 面板实例 (Page 将接管其所有权)
     * @details 会自动将 Panel 添加到 UI 栈，并将名字添加到下拉框
     */
    void _RegisterAlgorithm(AlgorithmPanelBase *panel);

  private slots:
    void OnModuleChanged(int index);

  private:
    void _FinalizeTask(bool success);

    QComboBox *_AlgoSelectCombo = nullptr;
    QStackedWidget *_PanelStack = nullptr;

    QThread *_TaskThread = nullptr;
    bool _IsExecuting = false;
};

} // namespace UI::Pages
