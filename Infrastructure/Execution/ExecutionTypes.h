#pragma once

#include <QMetaType>
#include <QString>
#include <QStringList>

#include <functional>
#include <utility>

namespace Infrastructure::Execution {

struct ValidationIssue {
    QString Title;
    QString Message;
};

class ExecutionLogSink {
  public:
    using Callback = std::function<void(const QString &)>;

    ExecutionLogSink() = default;
    explicit ExecutionLogSink(Callback callback) : _Callback(std::move(callback)) {}

    void Post(const QString &message) const {
        if (_Callback) {
            _Callback(message);
        }
    }

    explicit operator bool() const {
        return static_cast<bool>(_Callback);
    }

  private:
    Callback _Callback;
};

struct ExecutionResult {
    bool Success = false;
    QString Message;
    QStringList SavedPaths;
    int WarningCount = 0;
};

} // namespace Infrastructure::Execution

Q_DECLARE_METATYPE(Infrastructure::Execution::ExecutionResult)
