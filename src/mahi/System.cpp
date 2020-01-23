#include <mahi/System.hpp>
#include <nfd.h>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace mahi::gui::System {

DialogResult saveDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath) {
    nfdchar_t *savePath = NULL;
    nfdresult_t result = NFD_SaveDialog(filterList.c_str(), defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &savePath );
    if ( result == NFD_OKAY )
    {
        outPath = savePath;
        free(savePath);
        return DialogResult::Okay;
    }
    else if ( result == NFD_CANCEL )
        return DialogResult::Cancel;        
    else        
        return DialogResult::Error;
}

DialogResult openDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath) {
    nfdchar_t *openPath = NULL;
    nfdresult_t result = NFD_OpenDialog(filterList.c_str(), defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &openPath );
    if ( result == NFD_OKAY )
    {
        outPath = openPath;
        free(openPath);
        return DialogResult::Okay;
    }
    else if ( result == NFD_CANCEL )
        return DialogResult::Cancel;
    else 
        return DialogResult::Error;
}

DialogResult openDialog(const std::string& filterList, const std::string& defaultPath, std::vector<std::string>& outPaths) {    
    nfdpathset_t pathSet;
    nfdresult_t result = NFD_OpenDialogMultiple(filterList.c_str(), defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &pathSet);
    if ( result == NFD_OKAY )
    {
        std::size_t n = NFD_PathSet_GetCount(&pathSet);
        outPaths.resize(n);
        for (std::size_t i = 0; i < n; ++i )    
            outPaths[i] = NFD_PathSet_GetPath(&pathSet, i);        
        NFD_PathSet_Free(&pathSet);
        return DialogResult::Okay;
    }
    else if ( result == NFD_CANCEL )
        return DialogResult::Cancel;
    else 
        return DialogResult::Error;
}

DialogResult pickFolder(const std::string& defaultPath, std::string& outPath) {
    nfdchar_t *pickPath = NULL;
    nfdresult_t result = NFD_PickFolder( defaultPath.length() > 0 ? defaultPath.c_str() : NULL, &pickPath );
    if ( result == NFD_OKAY ) {
        outPath = pickPath;
        free(pickPath);
        return DialogResult::Okay;
    }
    else if ( result == NFD_CANCEL )
        return DialogResult::Cancel;
    else 
        return DialogResult::Error;
}

#ifdef _WIN32

void openUrl(const std::string& url) {
    ShellExecuteA(0, 0, url.c_str(), 0, 0 , 5);
}

void openEmail(const std::string& address, const std::string& subject) {
    std::string str = "mailto:" + address;
    if (!subject.empty()) 
        str += "?subject=" + subject;
    ShellExecuteA(0, 0, str.c_str(), 0, 0 , 5);
}

#else

void openUrl(const std::string& url) {
    // TODO
}

void openEmail(const std::string& address, const std::string& subject) {
    // TODO
}

#endif

} // namespace mahi::gui::System