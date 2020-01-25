#pragma once
#include <string>
#include <vector>

namespace mahi::gui::System {

/// Result of a file dialog function
enum DialogResult {
    Error,
    Okay,
    Cancel
};

/// File save dialog
DialogResult saveDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);

/// Single file open dialog
DialogResult openDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);

/// Multiple file open dialog
DialogResult openDialog(const std::string& filterList, const std::string& defaultPath, std::vector<std::string>& outPaths);

/// Folder selection dialog
DialogResult pickFolder(const std::string& defaultPath, std::string& outPath);

/// Opens an external link to a URL
void openUrl(const std::string& url);

/// Opens an external link to an email
void openEmail(const std::string& address, const std::string& subject = "");

/// Gets the CPU core usage as a percent used by all processes
double cpuUsageTotal();

/// Gets the CPU core usage as a percent used by this process
double cpuUsageProcess();

/// Gets the total virtual memory available in bytes
std::size_t virtMemAvailable();

/// Gets the virtual memory used by all processes in bytes
std::size_t virtMemUseTotal();

/// Gets the virtual memory used by this process in bytes
std::size_t virtMemUsedProcess();

/// Gets the total RAM available in bytes
std::size_t ramAvailable();

/// Gets the RAM used by all process in bytes
std::size_t ramUsedTotal();

/// Gets the RAM used by this process in bytes
std::size_t ramUsedProcess();


} // namespace mahi::gui::System