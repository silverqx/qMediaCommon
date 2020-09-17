#ifndef COMMONGLOBAL_H
#define COMMONGLOBAL_H

#include <QByteArray>
#include <QString>

#include <qt_windows.h>

#ifdef PROJECT_QBITTORRENT
#include "base/bittorrent/infohash.h"
#endif

enum UserMessages
{
    MSG_QBITTORRENT_UP            = WM_USER + 1,
    MSG_QBITTORRENT_DOWN          = WM_USER + 2,
    MSG_QBT_TORRENTS_CHANGED      = WM_USER + 3,
    MSG_QBT_TORRENT_REMOVED       = WM_USER + 4,
    MSG_QBT_TORRENTS_ADDED        = WM_USER + 5,
    MSG_QBT_TORRENT_MOVED         = WM_USER + 6,

    MSG_QMEDIA_UP                 = WM_USER + 100,
    MSG_QMEDIA_DOWN               = WM_USER + 101,
    MSG_QMD_DELETE_TORRENT        = WM_USER + 102,
    MSG_QMD_APPLICATION_ACTIVE    = WM_USER + 103,
    MSG_QMD_APPLICATION_DEACTIVE  = WM_USER + 104,
    MSG_QMD_PAUSE_TORRENT         = WM_USER + 105,
    MSG_QMD_RESUME_TORRENT        = WM_USER + 106,
    MSG_QMD_FORCE_RESUME_TORRENT  = WM_USER + 107,
    MSG_QMD_FORCE_RECHECK_TORRENT = WM_USER + 108,
};

static inline const auto IpcSendByteArray =
        [](const HWND hwnd, const UserMessages message, const QByteArray &byteArray)
{
    COPYDATASTRUCT copyDataStruct;
    copyDataStruct.lpData = static_cast<void *>(const_cast<char *>(byteArray.data()));
    copyDataStruct.cbData = byteArray.size();
    copyDataStruct.dwData = NULL;

    return ::SendMessage(hwnd, WM_COPYDATA,
                  (WPARAM) message,
                  (LPARAM) (LPVOID) &copyDataStruct);
};

Q_DECL_UNUSED
static inline const auto IpcSendString =
        [](const HWND hwnd, const UserMessages message, const QString &string)
{
    return ::IpcSendByteArray(hwnd, message, string.toUtf8().constData());
};

#ifdef PROJECT_QBITTORRENT
Q_DECL_UNUSED
static inline const auto IpcSendInfoHash =
        [](const HWND hwnd, const UserMessages message, const BitTorrent::InfoHash &infoHash)
{
    return ::IpcSendByteArray(hwnd, message, static_cast<QString>(infoHash).toUtf8().constData());
};
#endif

Q_DECL_UNUSED
static inline const auto IpcSendStdString =
        [](const HWND hwnd, const UserMessages message, const std::string &string)
{
    return ::IpcSendByteArray(hwnd, message, QByteArray::fromStdString(string));
};

#endif // COMMONGLOBAL_H
