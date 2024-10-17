#pragma once

#include "Internal/FileWatcher.hpp"

namespace zrn
{

class FileWatcher
{
public:
    FileWatcher() = default;

    bool Create(const FileWatcherConfig& config);
    void Destroy();
    void Poll();

    void SetFileCreatedCallback(Internal::FileCreatedFun callback);
    void SetFileDeletedCallback(Internal::FileDeletedFun callback);
    void SetFileModifiedCallback(Internal::FileModifiedFun callback);
    void SetFileRenamedCallback(Internal::FileRenamedFun callback);

private:
    Internal::FileWatcher m_Watcher;
};

inline bool FileWatcher::Create(const FileWatcherConfig& config)
{
    return Internal::Platform::CreateFilewatcher(&m_Watcher, &config);
}

inline void FileWatcher::Destroy()
{
    Internal::Platform::DestroyFilewatcher(&m_Watcher);
}

inline void FileWatcher::Poll()
{
    Internal::Platform::PollFileWatcher(&m_Watcher);
}

inline void FileWatcher::SetFileCreatedCallback(Internal::FileCreatedFun callback)
{
    m_Watcher.FileCreatedCallback = callback;
}

inline void FileWatcher::SetFileDeletedCallback(Internal::FileDeletedFun callback)
{
    m_Watcher.FileDeletedCallback = callback;
}

inline void FileWatcher::SetFileModifiedCallback(Internal::FileModifiedFun callback)
{
    m_Watcher.FileModifiedCallback = callback;
}

inline void FileWatcher::SetFileRenamedCallback(Internal::FileRenamedFun callback)
{
    m_Watcher.FileRenamedCallback = callback;
}


} // namespace zrn
