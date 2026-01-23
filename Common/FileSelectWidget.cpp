#include "FileSelectWidget.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

FileSelectWidget::FileSelectWidget(const QString &labelText,
                                   const QString &filter,
                                   Mode mode,
                                   QWidget *parent)
    : QWidget(parent), m_mode(mode), m_filter(filter) {
    // 采用垂直布局：上面是 Label，下面是 Input + Button
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 消除多余边距，使其能嵌入其他布局
    mainLayout->setSpacing(2);                  // 标题和输入框紧凑一些

    m_label = new QLabel(labelText, this);
    mainLayout->addWidget(m_label);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(5);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    m_pathEdit = new QLineEdit(this);
    m_browseBtn = new QPushButton(tr("浏览..."), this);
    m_browseBtn->setFixedWidth(70); // 固定按钮宽度

    inputLayout->addWidget(m_pathEdit);
    inputLayout->addWidget(m_browseBtn);

    mainLayout->addLayout(inputLayout);

    // 连接信号槽
    connect(m_browseBtn, &QPushButton::clicked, this, &FileSelectWidget::onBrowseClicked);
    connect(m_pathEdit, &QLineEdit::textChanged, this, &FileSelectWidget::pathChanged);

    // 设置对话框默认标题
    switch (m_mode) {
    case Mode::FileSave:
        m_dialogTitle = tr("保存文件");
        break;
    case Mode::Directory:
        m_dialogTitle = tr("选择目录");
        break;
    default:
        m_dialogTitle = tr("选择文件");
        break;
    }
}

QString FileSelectWidget::currentPath() const {
    return m_pathEdit->text().trimmed();
}

void FileSelectWidget::setPath(const QString &path) {
    m_pathEdit->setText(path);
}

void FileSelectWidget::setPlaceholderText(const QString &text) {
    m_pathEdit->setPlaceholderText(text);
}

void FileSelectWidget::onBrowseClicked() {
    QString path;
    QString currentDir = m_pathEdit->text().isEmpty() ? "" : m_pathEdit->text();

    if (m_mode == Mode::FileOpen) {
        path = QFileDialog::getOpenFileName(this, m_dialogTitle, currentDir, m_filter);
    } else if (m_mode == Mode::FileSave) {
        path = QFileDialog::getSaveFileName(this, m_dialogTitle, currentDir, m_filter);
    } else if (m_mode == Mode::Directory) {
        path = QFileDialog::getExistingDirectory(this, m_dialogTitle, currentDir);
    }

    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}