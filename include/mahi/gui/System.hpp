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

/// Opens a native file save dialog
DialogResult saveDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);

/// Opens a native single file open dialog
DialogResult openDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);

/// Opens a native multiple file open dialog
DialogResult openDialog(const std::string& filterList, const std::string& defaultPath, std::vector<std::string>& outPaths);

/// Opens a native folder selection dialog
DialogResult pickFolder(const std::string& defaultPath, std::string& outPath);

/// Opens a folder in the native file explorer
bool openFolder(const std::string& path);

/// Opens a file with the default application
bool openFile(const std::string& path);

/// Opens an external link to a URL
void openUrl(const std::string& url);

/// Opens an external link to an email
void openEmail(const std::string& address, const std::string& subject = "");

/// Gets the CPU core usage as a percent [0 to 1] used by all processes
double cpuUsageTotal();

/// Gets the CPU core usage as a percent [0 to 1] used by this process
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

/// Gets string name of the OS
const std::string& osName();

/// Gets string representation of the OS version
const std::string& osVersion();

/// Sleeps the thread for a given amount of time as accurately as possible
void sleep(double seconds);

} // namespace mahi::gui::System