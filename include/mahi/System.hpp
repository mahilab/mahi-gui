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

} // namespace mahi::gui::System