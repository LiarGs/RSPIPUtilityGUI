# RSPIPUtility 后期维护规则

这份文档的目标不是讲原理，而是给后续维护一个可直接执行的规则集。  
核心原则很简单：**先删重复，再加功能；先放对位置，再谈抽象。**

## 1. 新增算法时的标准流程

如果要新增一个算法，按下面顺序做：

1. 明确它属于哪个模块。
2. 明确它的 `AlgorithmId`、显示名、输出模式和排序。
3. 新增对应的 request 文件。
4. 新增对应的参数面板。
5. 在 `ModuleRegistry` 里注册。
6. 把新文件加入 `CMakeLists.txt`。
7. 编译并手工跑一遍最小流程。

### 1.1 新增 request

request 放在 `Application/Execution` 下，并按算法族拆文件。

推荐规则：

- 只新增真正需要的 request 文件。
- 如果属于已有算法族，就加入对应 family 文件。
- 如果是新算法族，再考虑新建独立 request 文件组。
- 具体执行逻辑写在 `Execute(const ExecutionContext &context) const` 里。

不要把所有 request 再塞回一个大文件里。

### 1.2 新增 panel

panel 放在 `Panels/<Module>/` 下，只负责：

- 生成界面。
- 收集参数。
- 校验输入。
- 构造 request。

不要在 panel 里直接做这些事情：

- 读图。
- 保存文件。
- 调底层算法。
- 组织线程。

### 1.3 注册到模块表

所有模块名、算法名、排序和输出模式只在 `ModuleRegistry` 里登记。

新增算法时只改一个真相源：

- `ModuleDescriptor`
- `AlgorithmDescriptor`

不要在 panel、主窗口、页面类里各写一份相同信息。

### 1.4 更新构建文件

新文件必须加入 `CMakeLists.txt`。

如果忘记加，Qt Creator 里通常能编过部分文件，但最终链接或 moc 阶段会出问题。

## 2. 代码放置规则

### `UI`

只放界面壳、状态切换和日志展示。

包括：

- `MainWindow`
- `ModulePageBase`
- 当前页面的执行入口

### `Panels`

只放算法参数面板。

这里可以做：

- 控件布局。
- 参数收集。
- 参数校验。
- 轻量文本说明。

这里不要做：

- 文件 IO。
- 任务调度。
- 算法实现。
- 重复命名规则拼接。

### `Application`

只放业务执行相关的东西：

- 模块/算法注册表。
- request 对象。
- request 公共 helper。

### `Infrastructure`

只放执行基础设施：

- `ValidationIssue`
- `ExecutionResult`
- `ExecutionLogSink`
- `TaskExecutor`

## 3. 禁止项

下面这些设计不要再回来了：

- `dynamic_cast` 分发。
- 空壳 `*Page` 类。
- panel 内直接执行算法。
- panel 内直接做文件保存。
- 重复维护模块名和算法名。
- 一个超大的 `AlgorithmRequest` 单文件。
- 为了“将来可能会用到”而提前堆复杂抽象。

如果某个抽象没有稳定职责，而且只有一层代码在用，优先考虑删除或合并。

## 4. 命名规则

### 4.1 模块 ID

模块 ID 使用小写下划线风格，例如：

- `preprocess`
- `mosaic`
- `reconstruct`
- `color_balance`
- `cloud_detection`
- `evaluation`

### 4.2 算法 ID

算法 ID 也使用小写下划线风格，例如：

- `geo_coordinate_align`
- `adaptive_isophote_patch`
- `boundary_gradient`

### 4.3 显示名

显示名可以中英混排，但要保持稳定，避免在不同地方出现多个版本。

### 4.4 request / panel 类名

类名尽量和功能对齐，例如：

- `BoundaryGradientEvaluationRequest`
- `GeoCoordinateAlignPanel`

不要起过于泛的名字，也不要把历史概念带回来。

### 4.5 输出命名

输出文件名前缀要稳定、短、能看懂。

规则建议：

- 目录输出用目录语义，不要伪装成单文件。
- 单文件输出就让 `SavePath` 直接表达最终文件。
- 同一种算法的输出命名不要在多个地方分别拼接。

## 5. 维护检查清单

每次改完代码，至少检查下面几项：

### 5.1 输入校验

- 空输入是否被拦截。
- 数量不匹配是否被拦截。
- 必填项是否有明确错误提示。
- 错误提示是否能让用户直接知道该改什么。

### 5.2 输出行为

- 输出模式是否和注册表一致。
- 文件路径和目录路径是否分别处理正确。
- 保存结果是否写进 `ExecutionResult::SavedPaths`。

### 5.3 日志风格

- 成功、失败、警告的语言是否统一。
- 日志是否足够短，但又足够说明问题。
- 执行过程中的错误是否只在一个地方处理。

### 5.4 边界条件

- 空面板。
- 空保存路径。
- 缺少参考图。
- 缺少掩膜。
- 带宽或 band 越界。
- 输入数量和预期数量不一致。

### 5.5 结构一致性

- `ModuleRegistry` 是否是唯一元信息来源。
- panel 是否还在重复保存算法信息。
- 是否引入了新的分发层。
- 是否让 UI 反过来依赖业务执行细节。

## 6. 变更准则

优先级按这个顺序来：

1. 删重复。
2. 合并同类项。
3. 保持数据流单向。
4. 再考虑抽象。

如果一个改动只是为了“理论上的扩展性”，但当前没有真实需求，通常先不要做。

## 7. 推荐的维护习惯

- 每次新增功能前，先确认它属于哪一层。
- 新增 request 时，优先复用已有 helper。
- 新增 panel 时，只写 UI 相关逻辑。
- 新增注册项时，顺手检查顺序和显示名。
- 合并前，先确认没有把旧概念重新引入。

如果后面项目规模继续增长，可以再考虑更细的目录规范；但在当前阶段，**少层、少名词、少重复** 比什么都重要。

