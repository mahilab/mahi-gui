#pragma once
#include <string>
#include <vector>
#include <nfd.hpp>

namespace mahi {
namespace gui {

/// Result of a file dialog function
enum DialogResult {
    DialogError,
    DialogOkay,
    DialogCancel
};

typedef nfdu8filteritem_t DialogFilter;

/// Opens a native file save dialog
DialogResult save_dialog(std::string& out_path, const std::vector<DialogFilter>& filters = {});

/// Opens a native single file open dialog
DialogResult open_dialog(std::string& out_path, const std::vector<DialogFilter>& filters = {});

/// Opens a native multiple file open dialog
DialogResult open_dialog(std::vector<std::string>& out_paths, const std::vector<DialogFilter>& filters = {});

/// Opens a native folder selection dialog
DialogResult pick_folder(std::string& out_path);

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