#pragma once
#include <QWidget>

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
     * @brief 验证面板上的参数输入是否合法
     * @return true 合法, false 不合法 (Panel 内部应弹窗提示)
     */
    virtual bool ValidateInput() const = 0;

    /**
     * @brief 执行算法流程
     * @details 包含：读取数据 -> 构造算法 -> 执行 -> 保存结果
     * @param globalSavePath 用户在主界面指定的保存路径（如果为空，Panel 应自行生成临时路径）
     * @return true 执行成功
     */
    virtual bool Run(const QString &globalSavePath) = 0;

  protected:
    /**
     *
     */
    virtual void _SetupUi() = 0;

  signals:
    void LogMessage(const QString &msg);
};

} // namespace Panels