// HubLink.h - tiny header-only bridge between a Gr8Meow app and Gr8Meow Hub.
// C++17, Win32. No dependencies.
#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>
#include <string>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")

namespace HubLink {

// >>> CHANGE: where the Hub's releases live (opened if the Hub isn't installed)
inline constexpr wchar_t kHubReleasesUrl[] =
    L"https://github.com/YOUR_GITHUB_USERNAME/Gr8Meow-Hub/releases/latest";

inline constexpr wchar_t kAppsKey[] = L"Software\\Gr8MeowStudios\\Apps\\";
inline constexpr wchar_t kHubKey[]  = L"Software\\Gr8MeowStudios\\Hub";

inline bool IsSafeId(const wchar_t* id) {
    if (!id || !*id) return false;
    for (const wchar_t* p = id; *p; ++p) {
        wchar_t c = *p;
        bool ok = (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
                  (c >= L'0' && c <= L'9') || c == L'-' || c == L'_';
        if (!ok) return false;
    }
    return true;
}

inline bool SetStr(HKEY key, const wchar_t* name, const std::wstring& value) {
    return RegSetValueExW(key, name, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(value.c_str()),
        static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t))) == ERROR_SUCCESS;
}

// Call once at startup. Tells the Hub: "I'm installed, this version, here is my exe".
inline bool Register(const wchar_t* appId, const wchar_t* version) {
    if (!IsSafeId(appId) || !version) return false;

    wchar_t exe[MAX_PATH * 2];
    DWORD n = GetModuleFileNameW(nullptr, exe, ARRAYSIZE(exe));
    if (n == 0 || n >= ARRAYSIZE(exe)) return false;

    std::wstring exePath(exe);
    size_t slash = exePath.find_last_of(L"\\/");
    std::wstring dir = (slash == std::wstring::npos) ? L"" : exePath.substr(0, slash);

    std::wstring subKey = std::wstring(kAppsKey) + appId;
    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, subKey.c_str(), 0, nullptr, 0,
                        KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS)
        return false;

    bool ok = SetStr(key, L"Version", version) &&
              SetStr(key, L"ExePath", exePath) &&
              SetStr(key, L"InstallDir", dir);
    RegCloseKey(key);
    return ok;
}

// Optional: call from your uninstall routine.
inline void Unregister(const wchar_t* appId) {
    if (!IsSafeId(appId)) return;
    std::wstring subKey = std::wstring(kAppsKey) + appId;
    RegDeleteTreeW(HKEY_CURRENT_USER, subKey.c_str());
}

inline bool FileExists(const std::wstring& path) {
    DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

inline std::wstring HubExePath() {
    wchar_t buf[MAX_PATH * 2];
    DWORD size = sizeof(buf);
    if (RegGetValueW(HKEY_CURRENT_USER, kHubKey, L"ExePath", RRF_RT_REG_SZ,
                     nullptr, buf, &size) != ERROR_SUCCESS)
        return L"";
    std::wstring p(buf);
    if (p.size() < 5) return L"";
    std::wstring ext = p.substr(p.size() - 4);
    for (auto& c : ext) c = static_cast<wchar_t>(towlower(c));
    if (ext != L".exe" || !FileExists(p)) return L"";
    return p;
}

// Menu action: "Check for updates / Open Gr8Meow Hub".
// Opens the Hub focused on this app. If the Hub isn't installed, opens its download page.
inline bool OpenHub(const wchar_t* appId) {
    std::wstring hub = HubExePath();
    if (!hub.empty() && IsSafeId(appId)) {
        std::wstring args = std::wstring(L"--update ") + appId;
        HINSTANCE r = ShellExecuteW(nullptr, L"open", hub.c_str(), args.c_str(),
                                    nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(r) > 32) return true;
    }
    HINSTANCE r = ShellExecuteW(nullptr, L"open", kHubReleasesUrl, nullptr, nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(r) > 32;
}

} // namespace HubLink
