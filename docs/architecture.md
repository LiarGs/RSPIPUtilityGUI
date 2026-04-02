# RSPIPUtility 架构说明

这份文档描述的是 **当前代码库的真实结构**，不是历史演进记录。目标是让新接手的人可以直接回答三个问题：

1. 系统怎么跑。
2. 谁负责什么。
3. 数据怎么在各层之间流动。

## 1. 总体分层

项目当前按四层理解最清晰：

- `UI`
  - 主窗口与页面容器。
  - 负责模块切换、输出路径、日志展示、执行按钮状态。
- `Panels`
  - 各算法的参数面板。
  - 只做参数采集、参数校验、请求对象构造。
- `Application`
  - 模块与算法注册表。
  - 请求对象与请求执行逻辑。
  - 共享执行辅助函数。
- `Infrastructure`
  - 执行结果、校验问题、日志通道、任务执行器等基础设施。

这四层的方向是单向的：`UI -> Panels -> Application -> Infrastructure`。上层可以依赖下层，但不要反向穿透。

## 2. 运行主线

当前执行链路是：

`Panel -> CollectRequest -> request.Execute(context) -> TaskExecutor`

更具体一点：

1. `UI::MainWindow` 从 `ModuleRegistry` 读取模块和算法元数据，生成模块页面。
2. `UI::Pages::ModulePageBase` 负责承载当前模块的算法下拉框和参数面板。
3. 用户在某个 `Panels::*` 面板里填写参数。
4. 面板先做纯 UI 校验，校验失败时返回 `ValidationIssue`。
5. 面板通过 `CollectRequest(const QString &globalSavePath)` 生成具体请求对象。
6. `ModulePageBase` 把请求交给 `Infrastructure::Execution::TaskExecutor`。
7. `TaskExecutor` 在后台执行请求对象的 `Execute(context)`。
8. 请求对象内部完成读图、处理、保存，并返回 `ExecutionResult`。
9. `UI` 收到结果后更新日志、保存路径和界面状态。

## 3. UI 侧职责

### `UI/MainWindow`

主窗口只做“壳”和全局状态管理：

- 创建模块页面。
- 管理输出路径。
- 汇总日志。
- 接收页面执行完成结果。

它不负责具体算法逻辑，也不负责算法参数的细节校验。

### `UI/Pages/ModulePageBase`

`ModulePageBase` 是模块页的统一容器，不再需要一堆空壳 `*Page` 类。

它负责：

- 根据当前模块从注册表拿到算法列表。
- 创建当前模块下的参数面板。
- 统一触发执行。
- 统一处理 `ValidationIssue`。
- 统一接收 `ExecutionResult` 并转发给主窗口。

## 4. 注册中心

`Application::Registry::ModuleRegistry` 是模块和算法元信息的唯一来源。

### `ModuleDescriptor`

包含：

- `ModuleId`
- `ModuleDisplayName`
- `SortOrder`

### `AlgorithmDescriptor`

包含：

- `ModuleId`
- `AlgorithmId`
- `AlgorithmDisplayName`
- `OutputMode`
- `SortOrder`
- `PanelFactory`

### 当前模块顺序

当前模块顺序由 `SortOrder` 决定，实际顺序是：

1. `Preprocess`
2. `Mosaic`
3. `Reconstruct`
4. `ColorBalance`
5. `CloudDetection`
6. `Evaluation`

算法顺序也由各自的 `SortOrder` 决定，不要在 `Panel` 里重复写一份。

## 5. 请求模型

请求对象位于 `Application/Execution`，当前采用“请求自执行”模型。

### 基类

`AlgorithmRequest` 是最小抽象基类，核心字段只有：

- `SavePath`

核心行为只有：

- `Execute(const ExecutionContext &context) const`

### 请求分组

请求按算法族拆分到独立文件里，避免把所有算法塞进一个大文件。

当前结构是：

- `CloudDetectionRequest.*`
- `ColorBalanceRequest.*`
- `PreprocessRequest.*`
- `ReconstructRequest.*`
- `MosaicRequest.*`
- `EvaluationRequest.*`

### 公共辅助

跨请求共享的通用逻辑放在：

- `AlgorithmRequestBase.h`
- `AlgorithmRequestCommon.h/.cpp`

这里放的是公共基类、共享参数结构和执行辅助函数，不放具体页面逻辑。

## 6. 执行与结果

### `ExecutionContext`

执行上下文当前主要提供日志通道：

- `ExecutionContext::Log`

请求在执行过程中通过日志通道输出状态、警告和错误。

### `ExecutionResult`

执行结果的核心字段是：

- `Success`
- `Message`
- `SavedPaths`
- `WarningCount`

UI 层只根据这个结果更新界面，不要自己再猜测任务成败。

### `ValidationIssue`

校验问题统一使用：

- `Title`
- `Message`

它表示“输入不合法，当前任务不应继续执行”。

## 7. 面板的职责边界

`Panels::*` 目录里的类只做三件事：

1. 展示控件。
2. 收集参数。
3. 校验输入。

面板不应该：

- 直接读写文件。
- 直接调用底层算法库。
- 自己组织后台线程。
- 重复保存路径命名规则。
- 重复维护算法元信息。

## 8. 设计原则

这个项目当前的设计原则可以概括为一句话：

**less is more**

具体落地为：

- 优先删重复，不优先加抽象。
- 一个概念只保留一个真相源。
- 能由请求对象直接完成的事情，不再加中间分发层。
- 面板只负责 UI，执行只负责执行。
- 如果一个层没有稳定职责，就不要继续保留。

## 9. 建议阅读入口

如果要快速读懂项目，建议按这个顺序看：

1. `UI/MainWindow.cpp`
2. `UI/Pages/ModulePageBase.cpp`
3. `Application/Registry/ModuleRegistry.cpp`
4. `Panels/AlgorithmPanelBase.h`
5. `Application/Execution/AlgorithmRequestBase.h`
6. `Application/Execution/AlgorithmRequestCommon.h/.cpp`

