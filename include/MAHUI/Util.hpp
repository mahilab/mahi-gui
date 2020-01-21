#pragma once

#include <string>

namespace carnot {

/// Opens a URL in the OS's default web browser
void openUrl(const std::string& url);

/// Opens a mail-to dialog in the OS's default email client
void openEmail(const std::string& address, const std::string& subject = "");

} // namespace carnot