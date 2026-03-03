#pragma once
#include <QMetaObject>
#include <QThread>
#include <QWidget>
#include <functional>

namespace Panels {

/**
 * @brief 算法面板抽象基类
 * @note 遵循单一职责原则：具体的 Panel 负责具体算法的一切（参数UI、执行、IO）
 */
class AlgorithmPanelBase : public QWidget {
    Q_OBJECT
  public:
    explicit AlgorithmPanelBase(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~AlgorithmPanelBase() = default;

    virtual QString AlgorithmName() const = 0;

    /**
     * @brief 算法说明（默认占位，可由具体 Panel 覆写）
     */
    virtual QString AlgorithmDescription() const {
        return QString("【%1】\n\nTODO: 在此补充算法说明。\n")
            .arg(AlgorithmName());
    }

    /**
     * @brief 验证面板上的参数输入是否合法
     * @return true 合法, false 不合法 (Panel 内部应弹窗提示)
     */
    virtual bool ValidateInput() = 0;

    /**
     * @brief 构建后台任务（在 UI 线程采集参数，在后台线程执行耗时逻辑）
     */
    virtual std::function<bool()> BuildTask(const QString &globalSavePath) = 0;

    /**
     * @brief 线程安全日志输出
     */
    void PostLog(const QString &msg) {
        if (QThread::currentThread() == thread()) {
            emit LogMessage(msg);
            return;
        }
        QMetaObject::invokeMethod(this, [this, msg]() { emit LogMessage(msg); }, Qt::QueuedConnection);
    }

  protected:
    virtual void _SetupUi() = 0;

  signals:
    void LogMessage(const QString &msg);
};

} // namespace Panels

