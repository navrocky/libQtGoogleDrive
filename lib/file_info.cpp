#include "file_info.h"

namespace GoogleDrive
{

FileInfo::FileInfo(const QVariantMap &data)
    : data_(data)
{
}

FileInfo::FileInfo(const FileInfo &src)
    : data_(src.data_)
{
}

QString FileInfo::id() const
{
    return data_["id"].toString();
}

QString FileInfo::title() const
{
    return data_["title"].toString();
}

QString FileInfo::mimeType() const
{
    return data_["mimeType"].toString();
}

bool FileInfo::isFolder() const
{
    return mimeType() == "application/vnd.google-apps.folder";
}

QDateTime FileInfo::createdDate() const
{
    return QDateTime::fromString(data_["createdDate"].toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedDate() const
{
    return QDateTime::fromString(data_["modifiedDate"].toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedByMeDate() const
{
    return QDateTime::fromString(data_["modifiedByMeDate"].toString(), Qt::ISODate);
}

QDateTime FileInfo::lastViewedByMeDate() const
{
    return QDateTime::fromString(data_["lastViewedByMeDate"].toString(), Qt::ISODate);
}

bool FileInfo::isEditable() const
{
    return data_["editable"].toBool();
}

}
