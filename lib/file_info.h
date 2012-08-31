#ifndef GOOGLE_DRIVE_FILE_INFO_H
#define GOOGLE_DRIVE_FILE_INFO_H

#include <QVariant>
#include <QString>
#include <QDateTime>
#include <QList>

namespace GoogleDrive
{

class FileInfo
{
public:
    explicit FileInfo(const QVariantMap& data);
    explicit FileInfo(const FileInfo& src);

    QVariantMap rawData() const {return data_;}

    QString id() const;
    QString title() const;
    QString mimeType() const;
    bool isFolder() const;
    QDateTime createdDate() const;
    QDateTime modifiedDate() const;
    QDateTime modifiedByMeDate() const;
    QDateTime lastViewedByMeDate() const;
    bool isEditable() const;

private:
    QVariantMap data_;
};

typedef QList<FileInfo> FileInfoList;

}

#endif
