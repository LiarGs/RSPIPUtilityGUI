#pragma once

#include <QString>

#include <functional>
#include <optional>
#include <vector>

class QWidget;

namespace Panels {
class AlgorithmPanelBase;
}

namespace Application::Registry {

enum class OutputSelectionMode {
    FilePath,
    Directory
};

struct ModuleDescriptor {
    QString ModuleId;
    QString ModuleDisplayName;
    int SortOrder = 0;
};

struct AlgorithmDescriptor {
    QString ModuleId;
    QString AlgorithmId;
    QString AlgorithmDisplayName;
    OutputSelectionMode OutputMode = OutputSelectionMode::FilePath;
    int SortOrder = 0;
    std::function<Panels::AlgorithmPanelBase *(QWidget *parent)> PanelFactory;
};

const std::vector<ModuleDescriptor> &AllModules();
std::optional<ModuleDescriptor> FindModule(const QString &moduleId);
std::vector<AlgorithmDescriptor> AlgorithmsForModule(const QString &moduleId);

} // namespace Application::Registry
