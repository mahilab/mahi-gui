// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <nfd.hpp>
#include <string>
#include <vector>

namespace mahi {
namespace gui {

/// Result of a file dialog function
enum DialogResult { DialogError, DialogOkay, DialogCancel };

/// Filtetype filter, a friendly name string follow by a series of comma separated extensions
/// e.g. DialogFilter filter({"Audio Files","wav,mp3"});
typedef nfdu8filteritem_t DialogFilter;

/// Opens a native file save dialog
DialogResult save_dialog(std::string& out_path, 
                         const std::vector<DialogFilter>& filters = {},
                         const std::string& default_path = "",
                         const std::string& default_name = "");

/// Opens a native single file open dialog
DialogResult open_dialog(std::string& out_path, 
                         const std::vector<DialogFilter>& filters = {},
                         const std::string& default_path = "");

/// Opens a native multiple file open dialog
DialogResult open_dialog(std::vector<std::string>&        out_paths,
                         const std::vector<DialogFilter>& filters      = {},
                         const std::string&               default_path = "");

/// Opens a native folder selection dialog
DialogResult pick_dialog(std::string& out_path, const std::string& default_path = "");

enum SysDir {
    UserProfile,
    AppDataRoaming,
    AppDataLocal,
    AppDataTemp,
    ProgramData,
    ProgramFiles,
    ProgramFilesX86,
};

/// Returns the common application data directory
const std::string& sys_dir(SysDir dir);

/// Opens a folder in the native file explorer
bool open_folder(const std::string& path);

/// Opens a file with the default application
bool open_file(const std::string& path);

/// Opens an external link to a URL
void open_url(const std::string& url);

/// Opens an external link to an email
void open_email(const std::string& address, const std::string& subject = "");

}  // namespace gui
}  // namespace mahi