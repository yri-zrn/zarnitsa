#pragma once

#include "Common.hpp"

#include <windows.h>

namespace zrn::Internal
{

struct FileWatcherWin32
{
    HANDLE     Directory;
    OVERLAPPED Overlapped;
    DWORD      ReadBuffer[1024];
};

namespace PlatformWin32
{
    bool CreateFileWatcher(FileWatcher* watcher, const FileWatcherConfig* config);
    void DestroyFileWatcher(FileWatcher* watcher);
    void PollFileWatcher(FileWatcher* watcher);
    void _FileWatcherRead(FileWatcher* watcher);
};

} // namespace zrn::Internal