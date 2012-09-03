#include "file_info.h"

#include <QUrl>

namespace GoogleDrive
{

struct FileInfo::Impl
{
    Impl(const QVariantMap& d)
        : data(d)
    {
    }

    QVariantMap data;
};

FileInfo::FileInfo(const QVariantMap& data)
    : d(new Impl(data))
{
}

FileInfo::FileInfo(const FileInfo &src)
    : d(new Impl(src.d->data))
{
}

FileInfo::~FileInfo()
{
    delete d;
}

QVariantMap FileInfo::rawData() const
{
    return d->data;
}

QString FileInfo::id() const
{
    return d->data["id"].toString();
}

QString FileInfo::title() const
{
    return d->data["title"].toString();
}

QString FileInfo::mimeType() const
{
    return d->data["mimeType"].toString();
}

bool FileInfo::isFolder() const
{
    return mimeType() == "application/vnd.google-apps.folder";
}

QDateTime FileInfo::createdDate() const
{
    return QDateTime::fromString(d->data["createdDate"].toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedDate() const
{
    return QDateTime::fromString(d->data["modifiedDate"].toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedByMeDate() const
{
    return QDateTime::fromString(d->data["modifiedByMeDate"].toString(), Qt::ISODate);
}

QDateTime FileInfo::lastViewedByMeDate() const
{
    return QDateTime::fromString(d->data["lastViewedByMeDate"].toString(), Qt::ISODate);
}

bool FileInfo::isEditable() const
{
    return d->data["editable"].toBool();
}

QUrl FileInfo::downloadUrl() const
{
    return QUrl(d->data["downloadUrl"].toString());
}

}
