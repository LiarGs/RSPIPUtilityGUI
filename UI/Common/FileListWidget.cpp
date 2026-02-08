#include "FileListWidget.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace UI::Common {

FileListWidget::FileListWidget(const QString &labelText,
                               const QString &filter,
                               QWidget *parent)
    : QWidget(parent), _Filter(filter) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    // 1. 标题
    _Label = new QLabel(labelText, this);
    mainLayout->addWidget(_Label);

    // 2. 列表
    _ListWidget = new QListWidget(this);
    _ListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(_ListWidget);

    // 3. 按钮组
    QHBoxLayout *btnLayout = new QHBoxLayout();
    _AddBtn = new QPushButton(tr("添加文件..."), this);
    _ClearBtn = new QPushButton(tr("清空"), this);

    btnLayout->addWidget(_AddBtn);
    btnLayout->addWidget(_ClearBtn);
    btnLayout->addStretch(); // 按钮靠左对齐

    mainLayout->addLayout(btnLayout);

    connect(_AddBtn, &QPushButton::clicked, this, &FileListWidget::_OnAddClicked);
    connect(_ClearBtn, &QPushButton::clicked, this, &FileListWidget::_OnClearClicked);
}

QStringList FileListWidget::Files() const {
    QStringList list;
    for (int i = 0; i < _ListWidget->count(); ++i) {
        list << _ListWidget->item(i)->text();
    }
    return list;
}

void FileListWidget::Clear() {
    _ListWidget->clear();
    emit FileListChanged();
}

void FileListWidget::AddFiles(const QStringList &Files) {
    if (!Files.isEmpty()) {
        _ListWidget->addItems(Files);
        emit FileListChanged();
    }
}

void FileListWidget::_OnAddClicked() {
    QStringList Files = QFileDialog::getOpenFileNames(this, tr("选择文件"), "", _Filter);
    if (!Files.isEmpty()) {
        AddFiles(Files);
    }
}

void FileListWidget::_OnClearClicked() {
    Clear();
}

} // namespace UI::Common
