#include "Infrastructure/Execution/OutputPlanner.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

namespace Infrastructure::Execution {

namespace {

QString TempDirectory() {
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
}

QString NormalizeExtension(QString extension) {
    if (extension.startsWith('.')) {
        extension.remove(0, 1);
    }
    return extension;
}

} // namespace

bool EnsureDirectory(const QString &directoryPath,
                     const ExecutionLogSink &log) {
    if (directoryPath.trimmed().isEmpty()) {
        log.Post(QStringLiteral("错误: 输出目录无效。"));
        return false;
    }

    if (QDir().mkpath(directoryPath)) {
        return true;
    }

    log.Post(QString("错误: 无法创建输出目录: %1").arg(directoryPath));
    return false;
}

QString BuildAutoImageOutputPath(const QString &prefix,
                                 const QString &extension) {
    return BuildOutputPath(
        TempDirectory(),
        QString("%1_result_%2.%3")
            .arg(prefix,
                 QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"),
                 NormalizeExtension(extension)));
}

std::optional<OutputPlan> ResolveBatchOutputPlan(const QString &requestedPath,
                                                 const QString &defaultPrefix,
                                                 int itemCount,
                                                 const ExecutionLogSink &log) {
    OutputPlan plan;
    const QString trimmedPath = requestedPath.trimmed();

    if (trimmedPath.isEmpty()) {
        plan.OutputDir = TempDirectory();
        plan.OutputPrefix = defaultPrefix;
        log.Post(QString(">> 使用临时目录保存结果: %1").arg(plan.OutputDir));
    } else {
        const QFileInfo outputInfo(trimmedPath);
        const bool treatAsDirectory = outputInfo.isDir() ||
                                      (!outputInfo.exists() && outputInfo.suffix().isEmpty());
        if (treatAsDirectory) {
            plan.OutputDir = QDir::cleanPath(trimmedPath);
            plan.OutputPrefix = defaultPrefix;
        } else {
            plan.OutputDir = outputInfo.absolutePath();
            plan.OutputPrefix = outputInfo.completeBaseName().isEmpty()
                                    ? defaultPrefix
                                    : outputInfo.completeBaseName();
        }

        log.Post(QString(">> 使用指定输出目录: %1").arg(plan.OutputDir));
        plan.SingleExactOutput = (itemCount == 1 && !treatAsDirectory);
    }

    if (!EnsureDirectory(plan.OutputDir, log)) {
        return std::nullopt;
    }

    return plan;
}

std::optional<QString> ResolveStrictOutputDirectory(const QString &requestedPath,
                                                    const ExecutionLogSink &log) {
    const QString trimmedPath = requestedPath.trimmed();
    if (trimmedPath.isEmpty()) {
        const QString tempDir = TempDirectory();
        log.Post(QString(">> 使用临时目录保存结果: %1").arg(tempDir));
        if (!EnsureDirectory(tempDir, log)) {
            return std::nullopt;
        }
        return tempDir;
    }

    const QFileInfo outputInfo(trimmedPath);
    if ((outputInfo.exists() && !outputInfo.isDir()) ||
        (!outputInfo.exists() && !outputInfo.suffix().isEmpty())) {
        log.Post(QString("错误: 当前算法只接受输出目录，不能使用文件路径: %1").arg(trimmedPath));
        return std::nullopt;
    }

    const QString outputDir = QDir::cleanPath(trimmedPath);
    if (!EnsureDirectory(outputDir, log)) {
        return std::nullopt;
    }

    log.Post(QString(">> 使用指定输出目录: %1").arg(outputDir));
    return outputDir;
}

std::optional<QString> ResolveTextOutputPath(const QString &requestedPath,
                                             const QString &defaultPrefix,
                                             const ExecutionLogSink &log) {
    QString finalSavePath = requestedPath.trimmed();
    if (finalSavePath.isEmpty()) {
        finalSavePath = BuildOutputPath(
            TempDirectory(),
            QString("%1_%2.txt")
                .arg(defaultPrefix, QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")));
        log.Post(">> 使用自动生成的路径: " + finalSavePath);
    } else {
        const QFileInfo userInfo(finalSavePath);
        const QString suffix = userInfo.suffix().toLower();
        const QStringList imageSuffixes = {"tif", "tiff", "png", "jpg", "jpeg", "bmp"};

        if (suffix.isEmpty() || imageSuffixes.contains(suffix)) {
            const QString baseName = userInfo.completeBaseName().isEmpty()
                                         ? defaultPrefix
                                         : userInfo.completeBaseName();
            finalSavePath = userInfo.dir().filePath(baseName + ".txt");
            log.Post(">> 评估结果将保存为文本文件: " + finalSavePath);
        }
    }

    const QFileInfo outputInfo(finalSavePath);
    if (!EnsureDirectory(outputInfo.absolutePath(), log)) {
        return std::nullopt;
    }

    return finalSavePath;
}

bool SaveTextFile(const QString &content,
                  const QString &finalSavePath,
                  const ExecutionLogSink &log,
                  const QString &successTemplate,
                  const QString &failureTemplate) {
    if (finalSavePath.trimmed().isEmpty()) {
        log.Post(QStringLiteral("错误: 结果输出路径为空。"));
        return false;
    }

    const QFileInfo outputInfo(finalSavePath);
    if (!EnsureDirectory(outputInfo.absolutePath(), log)) {
        return false;
    }

    QFile outputFile(finalSavePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        log.Post(failureTemplate.arg(finalSavePath));
        return false;
    }

    QTextStream stream(&outputFile);
    stream << content;
    outputFile.close();

    log.Post(successTemplate.arg(finalSavePath));
    return true;
}

QString BuildOutputPath(const QString &outputDir,
                        const QString &fileName) {
    return QDir(outputDir).filePath(fileName);
}

QString BuildIndexedOutputFileName(const QString &prefix,
                                   int index,
                                   const QString &inputPath,
                                   const QString &extension) {
    const QFileInfo inputInfo(inputPath);
    return QString("%1_%2_%3.%4")
        .arg(prefix)
        .arg(index)
        .arg(inputInfo.completeBaseName())
        .arg(NormalizeExtension(extension));
}

QString BuildBasenameOutputFileName(const QString &prefix,
                                    const QString &inputPath,
                                    const QString &extension) {
    const QFileInfo inputInfo(inputPath);
    return QString("%1_%2.%3")
        .arg(prefix, inputInfo.completeBaseName(), NormalizeExtension(extension));
}

} // namespace Infrastructure::Execution
