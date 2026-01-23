#include "FileListWidget.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

FileListWidget::FileListWidget(const QString &labelText,
                               const QString &filter,
                               QWidget *parent)
    : QWidget(parent), m_filter(filter) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    // 1. 标题
    m_label = new QLabel(labelText, this);
    mainLayout->addWidget(m_label);

    // 2. 列表
    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(m_listWidget);

    // 3. 按钮组
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_addBtn = new QPushButton(tr("添加文件..."), this);
    m_clearBtn = new QPushButton(tr("清空"), this);

    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_clearBtn);
    btnLayout->addStretch(); // 按钮靠左对齐

    mainLayout->addLayout(btnLayout);

    connect(m_addBtn, &QPushButton::clicked, this, &FileListWidget::onAddClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &FileListWidget::onClearClicked);
}

QStringList FileListWidget::files() const {
    QStringList list;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        list << m_listWidget->item(i)->text();
    }
    return list;
}

void FileListWidget::clear() {
    m_listWidget->clear();
    emit fileListChanged();
}

void FileListWidget::addFiles(const QStringList &files) {
    if (!files.isEmpty()) {
        m_listWidget->addItems(files);
        emit fileListChanged();
    }
}

void FileListWidget::onAddClicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, tr("选择文件"), "", m_filter);
    if (!files.isEmpty()) {
        addFiles(files);
    }
}

void FileListWidget::onClearClicked() {
    clear();
}