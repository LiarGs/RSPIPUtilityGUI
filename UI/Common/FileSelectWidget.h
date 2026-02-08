#pragma once

#include <QString>
#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;

namespace UI::Common {

/**
 * @brief 通用文件选择控件 (Reusable UI Component)
 * @details 封装了 "Label(标题) + LineEdit(路径) + Button(浏览)" 的组合
 * 符合单一职责原则 (SRP) 和 DRY (Don't Repeat Yourself) 原则
 */
class FileSelectWidget : public QWidget {
    Q_OBJECT
  public:
    enum class Mode {
        FileOpen, // 打开文件 (默认)
        FileSave, // 保存文件
        Directory // 选择目录
    };

    /**
     * @brief 构造函数
     * @param labelText 标题文字
     * @param filter 文件过滤器 (例如 "Images (*.tif *.png)")
     * @param mode 选择模式
     */
    explicit FileSelectWidget(const QString &labelText,
                              const QString &filter = "",
                              Mode mode = Mode::FileOpen,
                              QWidget *parent = nullptr);

    // 获取当前路径
    QString CurrentPath() const;

    // 设置路径
    void SetPath(const QString &path);

    // 设置输入框占位符
    void SetPlaceholderText(const QString &text);

  signals:
    // 当路径发生变化时触发 (无论是手动输入还是通过按钮选择)
    void PathChanged(const QString &newPath);

  private slots:
    void OnBrowseClicked();

  private:
    Mode _Mode;
    QString _Filter;
    QString _DialogTitle;

    QLabel *_Label;
    QLineEdit *_PathEdit;
    QPushButton *_BrowseBtn;
};

} // namespace UI::Common