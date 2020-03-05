#pragma once
#include <string>
#include <vector>

namespace mahi {
namespace gui {

/// Result of a file dialog function
enum DialogResult {
    DialogError,
    DialogOkay,
    DialogCancel
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

} // namespace gui
} // namespace mahi