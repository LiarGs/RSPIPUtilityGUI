#pragma once

#include "Infrastructure/Execution/ExecutionTypes.h"

namespace Application::Execution {

struct ExecutionContext {
    Infrastructure::Execution::ExecutionLogSink Log;
};

} // namespace Application::Execution
