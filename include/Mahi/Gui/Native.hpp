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
DialogResult save_dialog(const std::string& filter_list, const std::string& default_path, std::string& out_path);

/// Opens a native single file open dialog
DialogResult open_dialog(const std::string& filter_list, const std::string& default_path, std::string& out_path);

/// Opens a native multiple file open dialog
DialogResult open_dialog(const std::string& filter_list, const std::string& default_path, std::vector<std::string>& out_paths);

/// Opens a native folder selection dialog
DialogResult pick_folder(const std::string& default_path, std::string& out_path);

/// Opens a folder in the native file explorer
bool open_folder(const std::string& path);

/// Opens a file with the default application
bool open_file(const std::string& path);

/// Opens an external link to a URL
void open_url(const std::string& url);

/// Opens an external link to an email
void open_email(const std::string& address, const std::string& subject = "");

} // namespace gui
} // namespace mahi