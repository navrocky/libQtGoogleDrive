#ifndef GOOGLE_DRIVE_FILE_INFO_H
#define GOOGLE_DRIVE_FILE_INFO_H

#include <QVariant>
#include <QString>
#include <QDateTime>
#include <QList>

namespace GoogleDrive
{

/**
* @brief File information
*
* It holds full file information struct in QVariantMap format and provides
* convenient methods for read fields and build this struct from scratch.
*
* @sa CommandFileList, CommandGet
*/
class FileInfo
{
public:
    FileInfo();
    explicit FileInfo(const QVariantMap& data);
    FileInfo(const FileInfo& src);
    ~FileInfo();

    FileInfo& operator=(const FileInfo& src);

    bool isEmpty() const;

    QVariantMap rawData() const;

    QString id() const;
    void setId(const QString&);

    QString title() const;
    void setTitle(const QString&);

    QString mimeType() const;
    bool isFolder() const;
    bool isRoot() const;
    QDateTime createdDate() const;
    QDateTime modifiedDate() const;
    QDateTime modifiedByMeDate() const;
    QDateTime lastViewedByMeDate() const;
    bool isEditable() const;
    QUrl downloadUrl() const;
    qint64 fileSize() const;

private:
    struct Impl;
    Impl* d;
};

/**
* @typedef FileInfoList
* @brief List of file's informations.
*/
typedef QList<FileInfo> FileInfoList;

}

#endif
