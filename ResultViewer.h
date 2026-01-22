#pragma once

#include <QWidget>
#include <QString>

class QGraphicsView;
class QGraphicsScene;
class QTextEdit;
class QStackedWidget;

/**
 * @brief 通用结果查看器
 * 根据文件类型自动切换显示模式 (图像/文本)
 */
class ResultViewer : public QWidget {
    Q_OBJECT
public:
    explicit ResultViewer(QWidget *parent = nullptr);

    // 加载并显示结果文件
    void loadResult(const QString &filePath);

    // 清空显示
    void clear();

private:
    void setupUi();
    bool isImageFile(const QString &path) const;
    bool isTextFile(const QString &path) const;

private:
    QStackedWidget *m_stack;

    // 模式 1: 图像查看
    QGraphicsView *m_imageView;
    QGraphicsScene *m_imageScene;

    // 模式 2: 文本查看
    QTextEdit *m_textView;
};
