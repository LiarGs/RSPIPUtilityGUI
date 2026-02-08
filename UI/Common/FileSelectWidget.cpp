#include "FileSelectWidget.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace UI::Common {

FileSelectWidget::FileSelectWidget(const QString &labelText,
                                   const QString &filter,
                                   Mode mode,
                                   QWidget *parent)
    : QWidget(parent), _Mode(mode), _Filter(filter) {
    // 采用垂直布局：上面是 Label，下面是 Input + Button
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 消除多余边距，使其能嵌入其他布局
    mainLayout->setSpacing(2);                  // 标题和输入框紧凑一些

    _Label = new QLabel(labelText, this);
    mainLayout->addWidget(_Label);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(5);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    _PathEdit = new QLineEdit(this);
    _BrowseBtn = new QPushButton(tr("浏览..."), this);
    _BrowseBtn->setFixedWidth(70); // 固定按钮宽度

    inputLayout->addWidget(_PathEdit);
    inputLayout->addWidget(_BrowseBtn);

    mainLayout->addLayout(inputLayout);

    // 连接信号槽
    connect(_BrowseBtn, &QPushButton::clicked, this, &FileSelectWidget::OnBrowseClicked);
    connect(_PathEdit, &QLineEdit::textChanged, this, &FileSelectWidget::PathChanged);

    // 设置对话框默认标题
    switch (_Mode) {
    case Mode::FileSave:
        _DialogTitle = tr("保存文件");
        break;
    case Mode::Directory:
        _DialogTitle = tr("选择目录");
        break;
    default:
        _DialogTitle = tr("选择文件");
        break;
    }
}

QString FileSelectWidget::CurrentPath() const {
    return _PathEdit->text().trimmed();
}

void FileSelectWidget::SetPath(const QString &path) {
    _PathEdit->setText(path);
}

void FileSelectWidget::SetPlaceholderText(const QString &text) {
    _PathEdit->setPlaceholderText(text);
}

void FileSelectWidget::OnBrowseClicked() {
    QString path;
    QString currentDir = _PathEdit->text().isEmpty() ? "" : _PathEdit->text();

    if (_Mode == Mode::FileOpen) {
        path = QFileDialog::getOpenFileName(this, _DialogTitle, currentDir, _Filter);
    } else if (_Mode == Mode::FileSave) {
        path = QFileDialog::getSaveFileName(this, _DialogTitle, currentDir, _Filter);
    } else if (_Mode == Mode::Directory) {
        path = QFileDialog::getExistingDirectory(this, _DialogTitle, currentDir);
    }

    if (!path.isEmpty()) {
        _PathEdit->setText(path);
    }
}

} // namespace UI::Common
