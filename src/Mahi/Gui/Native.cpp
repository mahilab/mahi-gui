#include <Mahi/Gui/Native.hpp>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <pdh.h>
#include <psapi.h>
#include <tchar.h>
#include <windows.h>
#else
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sysctl.h>
#endif

namespace fs = std::filesystem;

namespace mahi {
namespace gui {



DialogResult save_dialog(std::string& out_path, const std::vector<DialogFilter>& filters, const std::string& default_path, const std::string& default_name) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard nfdGuard;
    NFD::UniquePathU8 savePath;
    nfdresult_t result = NFD::SaveDialog(savePath, 
                                         filters.empty() ? nullptr : &filters[0], 
                                         (nfdfiltersize_t)filters.size(),
                                         default_path.empty() ? nullptr : defpath.string().c_str(),
                                         default_name.c_str());
    if (result == NFD_OKAY) {
        out_path = savePath.get();
        return DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult open_dialog(std::string& out_path, const std::vector<DialogFilter>& filters, const std::string& default_path) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard nfdGuard;
    NFD::UniquePathU8 openPath;
    nfdresult_t result = NFD::OpenDialog(openPath, 
                                         filters.empty() ? nullptr : &filters[0], 
                                         (nfdfiltersize_t)filters.size(), 
                                         default_path.empty() ? nullptr : defpath.string().c_str());
    if (result == NFD_OKAY) {
        out_path = openPath.get();
        return DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult open_dialog(std::vector<std::string>& out_paths, const std::vector<DialogFilter>& filters, const std::string& default_path) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard nfdGuard;
    NFD::UniquePathSet openPaths;   
    nfdresult_t result = NFD::OpenDialogMultiple(openPaths, 
                                                 filters.empty() ? nullptr : &filters[0], 
                                                 (nfdfiltersize_t)filters.size(),
                                                 default_path.empty() ? nullptr : defpath.string().c_str());
    if (result == NFD_OKAY) {
        nfdpathsetsize_t numPaths;
        NFD::PathSet::Count(openPaths, numPaths);
        out_paths.resize(numPaths);
        for (nfdpathsetsize_t i = 0; i < numPaths; ++i) {
            NFD::UniquePathSetPath path;
            NFD::PathSet::GetPath(openPaths, i, path);
            out_paths[i] = path.get();
        }
        return DialogResult::DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult pick_dialog(std::string &out_path, const std::string& default_path)
{
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard nfdGuard;
    NFD::UniquePathU8 openPath;
    nfdresult_t result = NFD::PickFolder(openPath, default_path.empty() ? nullptr : defpath.string().c_str());
    if (result == NFD_OKAY) {
        out_path = openPath.get();
        return DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

///////////////////////////////////////////////////////////////////////////////
// WINDOWS
///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

bool open_folder(const std::string &path)
{
    fs::path p(path);
    if (fs::exists(p) && fs::is_directory(p))
    {
        ShellExecuteA(NULL, "open", p.generic_string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
        return true;
    }
    return false;
}

bool open_file(const std::string &path)
{
    fs::path p(path);
    if (fs::exists(p) && fs::is_regular_file(p))
    {
        ShellExecuteA(NULL, "open", p.generic_string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
        return true;
    }
    return false;
}

void open_url(const std::string &url)
{
    ShellExecuteA(0, 0, url.c_str(), 0, 0, 5);
}

void open_email(const std::string &address, const std::string &subject)
{
    std::string str = "mailto:" + address;
    if (!subject.empty())
        str += "?subject=" + subject;
    ShellExecuteA(0, 0, str.c_str(), 0, 0, 5);
}

#elif (__APPLE__)

///////////////////////////////////////////////////////////////////////////////
// macOS
///////////////////////////////////////////////////////////////////////////////

bool open_folder(const std::string &path)
{
    fs::path p(path);
    if (fs::exists(p) && fs::is_regular_file(p))
    {
        std::string command = "open " + p.generic_string();
        system(command.c_str());
        return true;
    }
    return false;
}

bool open_file(const std::string &path)
{
    fs::path p(path);
    if (fs::exists(p) && fs::is_directory(p))
    {
        std::string command = "open " + p.generic_string();
        system(command.c_str());
        return true;
    }
    return false;
}

void open_url(const std::string &url)
{
    std::string command = "open " + url;
    system(command.c_str());
}

void open_email(const std::string &address, const std::string &subject)
{
    std::string mailTo = "mailto:" + address + "?subject=" + subject; // + "\\&body=" + bodyMessage;
    std::string command = "open " + mailTo;
    system(command.c_str());
}

#endif

} // namespace gui
} // namesapce mahi

// Links/Resources
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
