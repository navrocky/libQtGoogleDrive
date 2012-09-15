#ifndef GOOGLE_DRIVE_ABOUT_INFO_H
#define GOOGLE_DRIVE_ABOUT_INFO_H

#include <QVariant>

namespace GoogleDrive
{

class AboutInfoPrivate;

/**
 * @brief An item with user information and settings.
 * @sa https://developers.google.com/drive/v2/reference/about#resource
 */
class AboutInfo
{
public:
    AboutInfo();
    explicit AboutInfo(const QVariantMap& data);
    explicit AboutInfo(const AboutInfo& src);
    ~AboutInfo();
    AboutInfo& operator=(const AboutInfo&);

    /**
     * Access to the raw data of about item.
     */
    const QVariantMap& rawData() const;

    /**
     * The name of the current user.
     */
    QString name() const;

    /**
     * The id of the root folder.
     */
    QString rootFolderId() const;

    /**
     * The total number of quota bytes.
     */
    quint64 quotaBytesTotal() const;

    /**
     * The number of quota bytes used.
     */
    quint64 quotaBytesUsed() const;

    /**
     * The number of quota bytes used by trashed items.
     */
    quint64 quotaBytesUsedInTrash() const;

    /**
     * The largest change id.
     */
    int largestChangeId() const;

    /**
     * The domain sharing policy for the current user.
     */
    QString domainSharingPolicy() const;

    /**
     * The current user's ID as visible in the permissions collection.
     */
    QString permissionId() const;

    /**
     * A boolean indicating whether the authenticated app is installed by the authenticated user.
     */
    bool isCurrentAppInstalled() const;

private:
    AboutInfoPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(AboutInfo)
};

}

#endif
