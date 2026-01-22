#include "ResultViewer.h"
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTextEdit>
#include <QStackedWidget>
#include <QFileInfo>
#include <QImageReader>
#include <QFile>
#include <QTextStream>
#include <QDebug>

ResultViewer::ResultViewer(QWidget *parent) : QWidget(parent) {
    setupUi();
}

void ResultViewer::setupUi() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);

    // --- Page 0: 图像查看器 ---
    m_imageScene = new QGraphicsScene(this);
    m_imageView = new QGraphicsView(m_imageScene, this);
    m_imageView->setBackgroundBrush(QBrush(QColor(40, 40, 40)));
    m_imageView->setRenderHint(QPainter::Antialiasing);
    m_imageView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_imageView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_stack->addWidget(m_imageView);

    // --- Page 1: 文本查看器 ---
    m_textView = new QTextEdit(this);
    m_textView->setReadOnly(true);
    m_textView->setStyleSheet("background-color: #1e1e1e; color: #dcdcdc; font-family: Consolas; font-size: 11pt;");
    m_stack->addWidget(m_textView);

    layout->addWidget(m_stack);
}

void ResultViewer::loadResult(const QString &filePath) {
    clear();

    QFileInfo info(filePath);
    if (!info.exists()) {
        m_textView->setText("错误: 结果文件不存在 -> " + filePath);
        m_stack->setCurrentWidget(m_textView);
        return;
    }

    if (isImageFile(filePath)) {
        // 图像模式
        QImage image(filePath);
        if (image.isNull()) {
            m_textView->setText("错误: 无法加载图像 -> " + filePath);
            m_stack->setCurrentWidget(m_textView);
        } else {
            m_imageScene->addPixmap(QPixmap::fromImage(image));
            m_imageView->fitInView(m_imageScene->itemsBoundingRect(), Qt::KeepAspectRatio);
            m_stack->setCurrentWidget(m_imageView);
        }
    }
    else if (isTextFile(filePath)){
        // 默认为文本模式 (包括 .txt, .log, .json 或其他)
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            // 处理中文编码，假设是UTF-8，如果乱码需调整
            in.setEncoding(QStringConverter::Utf8);
            m_textView->setText(in.readAll());
            file.close();
        } else {
            m_textView->setText("无法读取文件内容。");
        }
        m_stack->setCurrentWidget(m_textView);
    }else {
        m_textView->setText("无法读取文件内容。");
        m_stack->setCurrentWidget(m_textView);
    }
}

void ResultViewer::clear() {
    m_imageScene->clear();
    m_textView->clear();
}

bool ResultViewer::isImageFile(const QString &path) const {
    // 获取 Qt 支持的所有图像格式 (包含 jpg, png, tif 等)
    static QByteArrayList supportedFormats = QImageReader::supportedImageFormats();
    QString ext = QFileInfo(path).suffix().toLower();
    return supportedFormats.contains(ext.toUtf8());
}

bool ResultViewer::isTextFile(const QString &path) const {
    QString ext = QFileInfo(path).suffix().toLower();
    return (ext == "txt" || ext == "log" || ext == "json" || ext == "xml" || ext == "report");
}
