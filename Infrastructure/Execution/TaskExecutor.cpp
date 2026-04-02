#include "Infrastructure/Execution/TaskExecutor.h"

#include "Application/Execution/ExecutionContext.h"

#include <QMetaObject>
#include <QThread>

namespace Infrastructure::Execution {

TaskExecutor::TaskExecutor(QObject *parent) : QObject(parent) {
    qRegisterMetaType<Infrastructure::Execution::ExecutionResult>("Infrastructure::Execution::ExecutionResult");
}

TaskExecutor::~TaskExecutor() {
    if (_CurrentThread) {
        _CurrentThread->quit();
        _CurrentThread->wait();
    }
}

bool TaskExecutor::IsRunning() const {
    return _IsRunning;
}

bool TaskExecutor::Start(std::unique_ptr<Application::Execution::AlgorithmRequest> request,
                         ExecutionLogSink logSink) {
    if (_IsRunning || !request) {
        return false;
    }

    _IsRunning = true;
    emit TaskStarted();

    auto sharedRequest = std::shared_ptr<Application::Execution::AlgorithmRequest>(std::move(request));
    auto *thread = QThread::create([this, sharedRequest = std::move(sharedRequest), logSink]() {
        const Application::Execution::ExecutionContext context{logSink};
        const ExecutionResult result = sharedRequest->Execute(context);
        QMetaObject::invokeMethod(this,
                                  [this, result]() { _Finalize(result); },
                                  Qt::QueuedConnection);
    });

    thread->setParent(this);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    _CurrentThread = thread;
    thread->start();
    return true;
}

void TaskExecutor::_Finalize(const ExecutionResult &result) {
    _IsRunning = false;
    _CurrentThread = nullptr;
    emit TaskFinished(result);
}

} // namespace Infrastructure::Execution
