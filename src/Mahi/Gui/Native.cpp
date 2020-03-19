#include <Mahi/Gui/Native.hpp>
#include <nfd.h>
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

DialogResult save_dialog(const std::string &filterList, const std::string &defaultPath, std::string &outPath)
{
    nfdchar_t *savePath = NULL;
    nfdresult_t result = NFD_SaveDialog(filterList.c_str(), defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &savePath);
    if (result == NFD_OKAY)
    {
        outPath = savePath;
        free(savePath);
        return DialogResult::DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult open_dialog(const std::string &filterList, const std::string &defaultPath, std::string &outPath)
{
    nfdchar_t *openPath = NULL;
    nfdresult_t result = NFD_OpenDialog(filterList.c_str(), defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &openPath);
    if (result == NFD_OKAY)
    {
        outPath = openPath;
        free(openPath);
        return DialogResult::DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult open_dialog(const std::string &filterList, const std::string &defaultPath, std::vector<std::string> &outPaths)
{
    nfdpathset_t pathSet;
    nfdresult_t result = NFD_OpenDialogMultiple(filterList.c_str(), defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &pathSet);
    if (result == NFD_OKAY)
    {
        std::size_t n = NFD_PathSet_GetCount(&pathSet);
        outPaths.resize(n);
        for (std::size_t i = 0; i < n; ++i)
            outPaths[i] = NFD_PathSet_GetPath(&pathSet, i);
        NFD_PathSet_Free(&pathSet);
        return DialogResult::DialogOkay;
    }
    else if (result == NFD_CANCEL)
        return DialogResult::DialogCancel;
    else
        return DialogResult::DialogError;
}

DialogResult pick_folder(const std::string &defaultPath, std::string &outPath)
{
    nfdchar_t *pickPath = NULL;
    nfdresult_t result = NFD_PickFolder(defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &pickPath);
    if (result == NFD_OKAY)
    {
        outPath = pickPath;
        free(pickPath);
        return DialogResult::DialogOkay;
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
