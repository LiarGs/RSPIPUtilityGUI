#pragma once

#include "Application/Execution/AlgorithmRequest.h"
#include "Infrastructure/Execution/ExecutionTypes.h"

#include <QObject>

#include <memory>

class QThread;

namespace Infrastructure::Execution {

class TaskExecutor : public QObject {
    Q_OBJECT
  public:
    explicit TaskExecutor(QObject *parent = nullptr);
    ~TaskExecutor() override;

    bool IsRunning() const;
    bool Start(std::unique_ptr<Application::Execution::AlgorithmRequest> request,
               ExecutionLogSink logSink);

  signals:
    void TaskStarted();
    void TaskFinished(const Infrastructure::Execution::ExecutionResult &result);

  private:
    void _Finalize(const Infrastructure::Execution::ExecutionResult &result);

    QThread *_CurrentThread = nullptr;
    bool _IsRunning = false;
};

} // namespace Infrastructure::Execution
