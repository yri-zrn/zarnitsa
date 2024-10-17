#include "FileWatcherWin32.hpp"

#include "Internal/FileWatcher.hpp"

#include <stdio.h>
#include <iostream>

namespace zrn::Internal
{

static void _BuildPath(char* destination, size_t destination_size, FileWatcher* watcher, FILE_NOTIFY_INFORMATION* notification)
{
    size_t path_length = notification->FileNameLength / sizeof(WCHAR);

    int utf8_length = ::WideCharToMultiByte(CP_UTF8,
                                            0,
                                            notification->FileName,
                                            (int)path_length,
                                            NULL,
                                            0,
                                            NULL,
                                            NULL);
    
    size_t watcher_path_length = strlen(watcher->Path);

    size_t full_length = watcher_path_length + 1 + utf8_length;

    strcpy_s(destination, destination_size, watcher->Path);

    char* file_part = destination + watcher_path_length + 1;

    ::WideCharToMultiByte(CP_UTF8,
                          0,
                          notification->FileName,
                          (int)path_length,
                          file_part,
                          utf8_length,
                          NULL,
                          NULL);
    destination[watcher_path_length] = '/';
    destination[full_length] = '\0';
};

// use with TEXT() macro
// if you want to use char*
static BOOL _FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
        return false;
    return !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool PlatformWin32::CreateFileWatcher(FileWatcher* watcher, const FileWatcherConfig* config)
{
    FileWatcherWin32* w = &watcher->NativeFileWatcher;
    memset(watcher->Path, '\0', sizeof(watcher->Path));
    strcpy_s(watcher->Path, sizeof(watcher->Path), config->Path);
    assert(strlen(config->Path) <= sizeof(watcher->Path)/sizeof(char));

    watcher->IncludeSubdirectories = config->Recursive;
    watcher->Blocking = config->Blocking;
    w->Directory = ::CreateFile(watcher->Path,
                                FILE_LIST_DIRECTORY,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                NULL);

    if (!w->Directory)
        return false;
    
    w->Overlapped.hEvent = ::CreateEvent(NULL, FALSE, 0, NULL);

    _FileWatcherRead(watcher);

    return true;
}

void PlatformWin32::DestroyFileWatcher(FileWatcher* watcher)
{
    FileWatcherWin32* w = &watcher->NativeFileWatcher;
    ::CloseHandle(w->Directory);
}

void PlatformWin32::PollFileWatcher(FileWatcher* watcher)
{
    FileWatcherWin32* w = &watcher->NativeFileWatcher;

    BOOL result = TRUE;

    static bool file_created = false;
    static char rename_old_name[256];
    static char last_source[256];
    static char source[256];

    memset(last_source, 0, sizeof(last_source));
    memset(rename_old_name, 0, sizeof(rename_old_name));
    memset(source, 0, sizeof(source));

    while (true)
    {
        // DWORD wait_result = ::WaitForSingleObject(w->Overlapped.hEvent, 0);
        // if (wait_result == WAIT_OBJECT_0)
        // {

        DWORD bytes_transferred;
        result = ::GetOverlappedResult(w->Directory,
                                       &w->Overlapped,
                                       &bytes_transferred,
                                       watcher->Blocking);

        if (result == FALSE)
            break;

        FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)w->ReadBuffer;

        for (;;)
        {
            switch (event->Action)
            {
                case FILE_ACTION_ADDED:
                {
                    if (watcher->FileCreatedCallback)
                    {
                        _BuildPath(source, sizeof(source), watcher, event);
                        watcher->FileCreatedCallback(watcher, source);
                    }
                    break;
                }
                case FILE_ACTION_REMOVED:
                {
                    if (watcher->FileDeletedCallback)
                    {
                        _BuildPath(source, sizeof(source), watcher, event);
                        watcher->FileDeletedCallback(watcher, source);
                    }
                    break;
                }
                case FILE_ACTION_MODIFIED:
                {
                    if (watcher->FileModifiedCallback)
                    {
                        _BuildPath(source, sizeof(source), watcher, event);
                        // HACK: FILE_ACTION_MODIFIED emits twice when modifying files
                        //       larger than 0 bytes
                        if (strcmp(last_source, source) == 0)
                        {
                            break;
                        }
                        else
                        {
                            watcher->FileModifiedCallback(watcher, source);
                            strcpy_s(last_source, sizeof(last_source), source);
                        }
                    }
                    break;
                }
                case FILE_ACTION_RENAMED_OLD_NAME:
                {
                    _BuildPath(rename_old_name, sizeof(rename_old_name), watcher, event);
                    break;
                }
                case FILE_ACTION_RENAMED_NEW_NAME:
                {
                    if (watcher->FileRenamedCallback)
                    {
                        _BuildPath(source, sizeof(source), watcher, event);
                        watcher->FileRenamedCallback(watcher, rename_old_name, source);
                    }
                    break;
                }
            }
            if (event->NextEntryOffset == 0)
                break;
            
            event = (FILE_NOTIFY_INFORMATION*)((char*)event + event->NextEntryOffset);
        }
        // }
        _FileWatcherRead(watcher);
    };
}

void PlatformWin32::_FileWatcherRead(FileWatcher* watcher)
{
    FileWatcherWin32* w = &watcher->NativeFileWatcher;
    ::ZeroMemory( &w->Overlapped, sizeof(w->Overlapped));

    BOOL result = ::ReadDirectoryChangesW(w->Directory,
                                          w->ReadBuffer,
                                          sizeof(w->ReadBuffer),
                                          watcher->IncludeSubdirectories ? TRUE : FALSE,
                                            FILE_NOTIFY_CHANGE_LAST_WRITE
                                          | FILE_NOTIFY_CHANGE_CREATION
                                          | FILE_NOTIFY_CHANGE_FILE_NAME
                                          | FILE_NOTIFY_CHANGE_DIR_NAME,
                                          NULL,
                                          &w->Overlapped,
                                          NULL);
    assert(result);
}

} // namespace zrn::Internal