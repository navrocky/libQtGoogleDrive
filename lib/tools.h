#ifndef GOOGLE_DRIVE_TOOLS_H
#define GOOGLE_DRIVE_TOOLS_H

#include <QString>
#include <QNetworkRequest>

namespace GoogleDrive
{
namespace Internal
{

void setRequestAccessToken(QNetworkRequest& request, const QString& accessToken);

}
}

#endif
