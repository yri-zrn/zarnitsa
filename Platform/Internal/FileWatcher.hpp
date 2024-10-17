#pragma once

#ifdef ZRN_WIN32
#include "Internal/Win32/FileWatcherWin32.hpp"
#define NATIVE_FILEWATCHER_TYPE FileWatcherWin32
#define NATIVE_PLATFORM_TYPE    PlatformWin32
#endif

namespace zrn::Internal
{

typedef void (*FileCreatedFun)(FileWatcher* watcher, const char* path);
typedef void (*FileDeletedFun)(FileWatcher* watcher, const char* path);
typedef void (*FileModifiedFun)(FileWatcher* watcher, const char* path);
typedef void (*FileRenamedFun)(FileWatcher* watcher, const char* old_name, const char* new_name);

struct FileWatcher
{
    FileWatcher() = default;

    char Path[260];
    bool IncludeSubdirectories;
    bool Blocking;

    FileCreatedFun  FileCreatedCallback  = nullptr;
    FileDeletedFun  FileDeletedCallback  = nullptr;
    FileModifiedFun FileModifiedCallback = nullptr;
    FileRenamedFun  FileRenamedCallback  = nullptr;
    NATIVE_FILEWATCHER_TYPE NativeFileWatcher;
};

namespace Platform
{

inline bool CreateFilewatcher(FileWatcher* filewatcher, const FileWatcherConfig* config)
{
    return NATIVE_PLATFORM_TYPE::CreateFileWatcher(filewatcher, config);
}

inline void DestroyFilewatcher(FileWatcher* filewatcher)
{
    NATIVE_PLATFORM_TYPE::DestroyFileWatcher(filewatcher);
}

inline void PollFileWatcher(FileWatcher* filewatcher)
{
    NATIVE_PLATFORM_TYPE::PollFileWatcher(filewatcher);
}

} // namespace Platform

} // namespace zrn::Internal