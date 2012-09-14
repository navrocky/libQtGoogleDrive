#include "file_info.h"

#include <QUrl>

namespace GoogleDrive
{

namespace
{
const char* cId = "id";
const char* cTitle = "title";
}

struct FileInfo::Impl
{
    Impl(const QVariantMap& d)
        : data(d)
    {
    }

    QVariantMap data;
};

FileInfo::FileInfo()
    : d(new Impl(QVariantMap()))
{
}

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

FileInfo &FileInfo::operator =(const FileInfo &src)
{
    d->data = src.d->data;
}

QVariantMap FileInfo::rawData() const
{
    return d->data;
}

QString FileInfo::id() const
{
    return d->data.value(cId).toString();
}

void FileInfo::setId(const QString& id)
{
    d->data[cId] = id;
}

QString FileInfo::title() const
{
    return d->data.value(cTitle).toString();
}

void FileInfo::setTitle(const QString& title)
{
    d->data[cTitle] = title;
}

QString FileInfo::mimeType() const
{
    return d->data.value("mimeType").toString();
}

bool FileInfo::isFolder() const
{
    return mimeType() == "application/vnd.google-apps.folder";
}
bool FileInfo::isRoot() const
{
    //FIXME works strange
    return d->data.value("parents").toList().isEmpty();
}

QDateTime FileInfo::createdDate() const
{
    return QDateTime::fromString(d->data.value("createdDate").toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedDate() const
{
    return QDateTime::fromString(d->data.value("modifiedDate").toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedByMeDate() const
{
    return QDateTime::fromString(d->data.value("modifiedByMeDate").toString(), Qt::ISODate);
}

QDateTime FileInfo::lastViewedByMeDate() const
{
    return QDateTime::fromString(d->data.value("lastViewedByMeDate").toString(), Qt::ISODate);
}

bool FileInfo::isEditable() const
{
    return d->data.value("editable").toBool();
}

QUrl FileInfo::downloadUrl() const
{
    return QUrl(d->data.value("downloadUrl").toString());
}

qint64 FileInfo::fileSize() const
{
    qint64 res = d->data.value("fileSize").toULongLong();
    return res;
}

}
