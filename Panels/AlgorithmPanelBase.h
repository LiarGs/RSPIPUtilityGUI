#pragma once

#include "Application/Execution/AlgorithmRequest.h"
#include "Infrastructure/Execution/ExecutionTypes.h"

#include <QWidget>

#include <memory>
#include <optional>

namespace Panels {

/**
 * @brief 算法面板抽象基类
 * @note Panel 负责参数编辑和 UI 校验，真正的执行由应用层 request 承担
 */
class AlgorithmPanelBase : public QWidget {
    Q_OBJECT
  public:
    explicit AlgorithmPanelBase(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~AlgorithmPanelBase() = default;

    virtual QString AlgorithmDescription() const {
        return QStringLiteral("当前算法说明暂未补充。");
    }

    virtual std::optional<Infrastructure::Execution::ValidationIssue> ValidateInput() = 0;

    virtual std::unique_ptr<Application::Execution::AlgorithmRequest>
    CollectRequest(const QString &globalSavePath) const = 0;

  protected:
    virtual void _SetupUi() = 0;
};

} // namespace Panels
