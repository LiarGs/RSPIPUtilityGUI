#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QStackedWidget>
#include <QLineEdit>
#include <QList>

// 引入新类
#include "ResultViewer.h"

class IAlgorithmPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAlgorithmChanged(int index);
    void onBrowseOutput();
    void onRunClicked();
    void onLogMessage(const QString &msg);

    // [修改]: 槽函数现在接收文件路径
    void onResultFileReady(const QString &filePath);

private:
    void setupUi();
    void initAlgorithms();

    // --- UI Controls ---
    QComboBox *m_algoSelector;
    QStackedWidget *m_paramStack;
    QLineEdit *m_outputPathEdit;
    QPushButton *m_runBtn;

    // [修改]: 使用通用的结果查看器替代单纯的 GraphicsView
    ResultViewer *m_resultViewer;

    QTextEdit *m_logConsole;

    // --- Logic ---
    QList<IAlgorithmPage*> m_pages;
};
