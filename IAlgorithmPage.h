#pragma once

#include <QWidget>

/**
 * @brief 算法页面接口 (Strategy Interface)
 */
class IAlgorithmPage : public QWidget {
    Q_OBJECT
public:
    explicit IAlgorithmPage(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~IAlgorithmPage() = default;

    virtual QString moduleName() const = 0;

    // 执行算法，savePath 可能为空
    virtual void execute(const QString &savePath) = 0;

signals:
    // [修改]: 算法执行完成，通知主界面加载该结果文件
    void resultFileReady(const QString &filePath);

    void logMessage(const QString &msg);
};
