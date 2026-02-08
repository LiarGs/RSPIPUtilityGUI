#pragma once

#include <QStringList>
#include <QWidget>

class QListWidget;
class QPushButton;
class QLabel;

namespace UI::Common {

/**
 * @brief 通用文件列表控件
 * @details 封装了 "Label + ListWidget + AddButton + ClearButton" 的组合
 * 用于需要选择多个文件的场景 (如 Mosaic)
 */
class FileListWidget : public QWidget {
    Q_OBJECT
  public:
    explicit FileListWidget(const QString &labelText,
                            const QString &filter = "",
                            QWidget *parent = nullptr);

    // 获取当前列表中的所有文件路径
    QStringList Files() const;

    // 清空列表
    void Clear();

    // 手动添加文件
    void AddFiles(const QStringList &Files);

  signals:
    void FileListChanged();

  private slots:
    void _OnAddClicked();
    void _OnClearClicked();

  private:
    QString _Filter;
    QLabel *_Label;
    QListWidget *_ListWidget;
    QPushButton *_AddBtn;
    QPushButton *_ClearBtn;
};

} // namespace UI::Common