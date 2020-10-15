#include <Mahi/Gui/Native.hpp>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <pdh.h>
#include <psapi.h>
#include <tchar.h>
#include <windows.h>
#else
#endif

// TODO: We need a more robust way to detect where fs may be...
#ifdef __linux__
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace mahi {
namespace gui {

DialogResult save_dialog(std::string& out_path, const std::vector<DialogFilter>& filters,
                         const std::string& default_path, const std::string& default_name) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard        nfdGuard;
    NFD::UniquePathU8 savePath;
    nfdresult_t       result = NFD::SaveDialog(
        savePath, filters.empty() ? nullptr : &filters[0], (nfdfiltersize_t)filters.size(),
        default_path.empty() ? nullptr : defpath.string().c_str(), default_name.c_str());
    if (result == NFD_OKAY) {
        out_path = savePath.get();
        return DialogOkay;
    } else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult open_dialog(std::string& out_path, const std::vector<DialogFilter>& filters,
                         const std::string& default_path) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard        nfdGuard;
    NFD::UniquePathU8 openPath;
    nfdresult_t       result = NFD::OpenDialog(openPath, filters.empty() ? nullptr : &filters[0],
                                         (nfdfiltersize_t)filters.size(),
                                         default_path.empty() ? nullptr : defpath.string().c_str());
    if (result == NFD_OKAY) {
        out_path = openPath.get();
        return DialogOkay;
    } else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult open_dialog(std::vector<std::string>&        out_paths,
                         const std::vector<DialogFilter>& filters,
                         const std::string&               default_path) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard         nfdGuard;
    NFD::UniquePathSet openPaths;
    nfdresult_t        result = NFD::OpenDialogMultiple(
        openPaths, filters.empty() ? nullptr : &filters[0], (nfdfiltersize_t)filters.size(),
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
    } else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult pick_dialog(std::string& out_path, const std::string& default_path) {
    fs::path defpath(default_path);
    defpath.make_preferred();
    NFD::Guard        nfdGuard;
    NFD::UniquePathU8 openPath;
    nfdresult_t       result =
        NFD::PickFolder(openPath, default_path.empty() ? nullptr : defpath.string().c_str());
    if (result == NFD_OKAY) {
        out_path = openPath.get();
        return DialogOkay;
    } else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

namespace {
struct SysDirStr {
    SysDirStr(const char* name) {
        fs::path p(std::string(std::getenv(name)));
        str = p.generic_string();
    }
    std::string str;
};
}  // namespace

///////////////////////////////////////////////////////////////////////////////
// WINDOWS
///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

const std::string& sys_dir(SysDir dir) {
    static SysDirStr user("USERPROFILE");
    static SysDirStr roam("APPDATA");
    static SysDirStr local("LOCALAPPDATA");
    static SysDirStr temp("TEMP");
    static SysDirStr data("PROGRAMDATA");
    static SysDirStr files("PROGRAMFILES");
    static SysDirStr x86("PROGRAMFILES(X86)");
    switch (dir) {
        case SysDir::UserProfile: return user.str; break;
        case SysDir::AppDataRoaming: return roam.str; break;
        case SysDir::AppDataLocal: return local.str; break;
        case SysDir::AppDataTemp: return temp.str; break;
        case SysDir::ProgramData: return data.str; break;
        case SysDir::ProgramFiles: return files.str; break;
        case SysDir::ProgramFilesX86: return x86.str; break;
    }
}

bool open_folder(const std::string& path) {
    fs::path p(path);
    if (fs::exists(p) && fs::is_directory(p)) {
        ShellExecuteA(NULL, "open", p.generic_string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
        return true;
    }
    return false;
}

bool open_file(const std::string& path) {
    fs::path p(path);
    if (fs::exists(p) && fs::is_regular_file(p)) {
        ShellExecuteA(NULL, "open", p.generic_string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
        return true;
    }
    return false;
}

void open_url(const std::string& url) { ShellExecuteA(0, 0, url.c_str(), 0, 0, 5); }

void open_email(const std::string& address, const std::string& subject) {
    std::string str = "mailto:" + address;
    if (!subject.empty())
        str += "?subject=" + subject;
    ShellExecuteA(0, 0, str.c_str(), 0, 0, 5);
}

#elif defined(__APPLE__)

///////////////////////////////////////////////////////////////////////////////
// macOS
///////////////////////////////////////////////////////////////////////////////

const std::string& sys_dir(SysDir dir) {
    static std::string todo = "TODO,SORRY";
    return todo;
}

bool open_folder(const std::string& path) {
    int      anErr = 0;
    fs::path p(path);
    if (fs::exists(p) && fs::is_regular_file(p)) {
        std::string command = "open " + p.generic_string();
        system(command.c_str());
        return true;
    }
    return false;
}

bool open_file(const std::string& path) {
    int      anErr = 0;
    fs::path p(path);
    if (fs::exists(p) && fs::is_directory(p)) {
        std::string command = "open " + p.generic_string();
        system(command.c_str());
        return true;
    }
    return false;
}

void open_url(const std::string& url) {
    int         anErr   = 0;
    std::string command = "open " + url;
    system(command.c_str());
}

void open_email(const std::string& address, const std::string& subject) {
    std::string mailTo =
        "mailto:" + address + "?subject=" + subject;  // + "\\&body=" + bodyMessage;
    std::string command = "open " + mailTo;
    system(command.c_str());
}

#elif defined(__linux__)

static int anErr = 0;

///////////////////////////////////////////////////////////////////////////////
// Linux
///////////////////////////////////////////////////////////////////////////////

const std::string& sys_dir(SysDir dir) {
    static std::string todo = "TODO,SORRY";
    return todo;
}

bool open_folder(const std::string& path) {
    fs::path p(path);
    if (fs::exists(p) && fs::is_regular_file(p)) {
        std::string command = "open " + p.generic_string();
        anErr               = system(command.c_str());
        if (anErr != 0)
            std::cout << "Pb with open_folder()"
                      << "\n";
        return true;
    }
    return false;
}

bool open_file(const std::string& path) {
    fs::path p(path);
    if (fs::exists(p) && fs::is_directory(p)) {
        std::string command = "open " + p.generic_string();
        anErr               = system(command.c_str());

        if (anErr != 0)
            std::cout << "Pb with open_file()"
                      << "\n";

        return true;
    }
    return false;
}

void open_url(const std::string& url) {
    std::string command = "open " + url;

    anErr = system(command.c_str());

    if (anErr != 0)
        std::cout << "Pb with open_url()"
                  << "\n";
}

void open_email(const std::string& address, const std::string& subject) {
    std::string mailTo =
        "mailto:" + address + "?subject=" + subject;  // + "\\&body=" + bodyMessage;
    std::string command = "open " + mailTo;
    anErr               = system(command.c_str());

    if (anErr != 0)
        std::cout << "Pb with open_url()"
                  << "\n";
}

#endif

}  // namespace gui
}  // namespace mahi

// Links/Resources
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
